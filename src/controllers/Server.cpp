/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmeziani <mmeziani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/27 01:54:20 by sel-kham          #+#    #+#             */
/*   Updated: 2023/09/15 12:43:01 by mmeziani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../modules/Server.hpp"

/* Constructors & Destructors */
Server::Server(void) { }

Server::Server(const int &port, const str_t &password): host("irc.LKHEDMAOUI_SERVER.com")
{
	this->port = port;
	this->password = password;
	this->masterSocketFd = -1;
	memset(&addr, 0, sizeof(sockaddr_in_t));
}

Server::~Server(void) { }

/* Getters & Setters */
const int	&Server::getMasterSocketFd(void) const
{
	return (this->masterSocketFd);
}

void		Server::setMasterSocketFd(const int &fd)
{
	this->masterSocketFd = fd;
}

const int	&Server::getPort(void) const
{
	return (this->port);
}

void		Server::setPort(const int &port)
{
	this->port = port;
}

const str_t	&Server::getPassword(void) const
{
	return (this->password);
}

void		Server::setPassword(const str_t &pass)
{
	this->password = pass;
}

const str_t	&Server::getHost(void) const
{
	return (this->host);
}

client_m::iterator    Server::getClientByNickname(const str_t &nickname)
{
    client_m::iterator    it = this->clients.begin();
    for (; it != this->clients.end(); it++)
        if (it->second.getNickname() == nickname)
            return (it);
    return (it);
}

void		Server::initSocketMaster(void)
{
	int	optval;

	optval = 1;
	this->masterSocketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->masterSocketFd < 0)
		throw std::runtime_error("Couldn't initialize server's socket");
	if (setsockopt(this->masterSocketFd, SOL_SOCKET, SO_REUSEADDR,  (char *) &optval, sizeof(optval)) < 0)
		throw std::runtime_error("Error while setting socket options");
	if (fcntl(this->masterSocketFd, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("Error while setting server's file socket to non-blocking");
}

void			Server::bindSocketMaster(void)
{
	int	res = 1;

	this->addr.sin_family = AF_INET;
	this->addr.sin_port = htons(this->port);
	this->addr.sin_addr.s_addr = htonl(INADDR_ANY);
	res = bind(this->masterSocketFd, (struct sockaddr *) &this->addr, sizeof(struct sockaddr));
	if (res < 0)
		throw std::runtime_error("Couldn't bind socket");
}

void			Server::listenForConnections(void)
{
	int	res;

	res = -1;
	res = listen(this->masterSocketFd, 5);
	if (res < 0)
		throw std::runtime_error("Error while listenning to connections");
}

int			Server::acceptConnections(void)
{
	socklen_t		size;
	int				newFd;
	Client	newClient = Client(-1);

	size = sizeof(this->addr);
	newFd = accept(this->masterSocketFd, (struct sockaddr *) &this->addr, &size);
	if (newFd < 0)
		return (perror("accept errot"), -1);
	newClient.setSocketFd(newFd);
	newClient.setHost(inet_ntoa(this->addr.sin_addr));
	newClient.setPort(ntohs(this->addr.sin_port));
	this->integrateNewConnect(newClient);
	return (newFd);
}

pollfd_t	Server::initPollFd(int fd, short event, short revent)
{
	pollfd_t	pfd;

	pfd.fd = fd;
	pfd.events = event;
	pfd.revents = revent;
	return (pfd);
}

void		Server::integrateNewConnect(Client &client)
{
	pollfd_t	pfd;

	memset(&pfd, 0, sizeof(pfd));
	pfd = this->initPollFd(client.getSocketFd(), POLLIN, 0);
	this->pfds.push_back(pfd);
	this->clients.insert(std::pair<const int, Client>(client.getSocketFd(), client));
}

void		Server::clean(const int &fd)
{
	client_m::iterator	it = this->clients.find(fd);
	str_t	nickname = "";

	if (it != this->clients.end())
	{
		nickname = it->second.getNickname();
		for (channel_m::iterator it2 = this->channels.begin(); it2 != this->channels.end(); it2++)
		{
			if(it2->second.isJoined(nickname))
				it2->second.removeClient(nickname);
			if(it2->second.isOperator(nickname))
				it2->second.removeMod(nickname);
		}
	}
	close(fd);
	this->clients.erase(fd);
	for (pollfd_v::iterator it = this->pfds.begin(); it != this->pfds.end(); it++)
	{
		if (it->fd == fd)
		{
			this->pfds.erase(it);
			break ;
		}
	}
}

int			Server::readRequest(Client &client)
{
	CommandWorker	cw = CommandWorker(this);
	int	res = 0;
	str_t	response;
	char	tmpbuff[1024];

	memset(tmpbuff, 0, sizeof(tmpbuff));
	res = recv(client.getSocketFd(), tmpbuff, sizeof(tmpbuff), 0);
	client.appendToMessage(tmpbuff);
	if (res <= 0)
		return (res);
	tmpbuff[res] = 0;
	if (client.getMessage().find(TRAILING) != str_t::npos)
	{	
		cw.setRequest(client.getMessage());
		cw.extractCommand();
		response = cw.execute(client);
		send(client.getSocketFd(), response.c_str(), response.size(), 0);
		client.setMessage("");
	}
	return (res);
}

void	Server::removeChannel(str_t channel)
{
	std::map<const str_t, Channel>::iterator channelIter = this->channels.find(channel);
	if (channelIter->first == channel)
	{
		client_n joinedClients = channelIter->second.getJoinedclients();
		if (!joinedClients.size())
			this->channels.erase(channel);
	}
}