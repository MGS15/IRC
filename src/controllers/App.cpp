/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   App.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmeziani <mmeziani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/27 01:37:35 by sel-kham          #+#    #+#             */
/*   Updated: 2023/09/15 12:44:19 by mmeziani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../modules/App.hpp"

/* Constructors & Destructors */
App::App(const char *port, const char *password)
{
	try
	{
		this->setPort(port);
		this->setPassword(password);
		this->server = Server(this->port, this->password);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		exit(EXIT_FAILURE);
	}
	
}

App::~App(void) { }

/* Internal class functionallities */
void		App::setPort(const char *port)
{
	unsigned long	found;

	if (!port)
		throw std::runtime_error("Port is required.");
	found = str_t(port).find_first_not_of("0123456789");
	if (found != std::string::npos)
		throw std::runtime_error("Port must contain only charachters.");
	found = atoi(port);
	if (found < 1024 || found > 65535)
		throw std::runtime_error("Port must be a number betweeen 1024 and 65535.");
	this->port = found;
}

void		App::setPassword(const char *password)
{
	if (!password)
		throw std::runtime_error("Password is required.");
	if (str_t(password).size() < 4)
		throw std::runtime_error("Password is required.");
	this->password = str_t(password);
}

/* External Class functionalities */
void	App::init(void)
{
	try
	{
		this->server.initSocketMaster();
		this->server.bindSocketMaster();
		this->server.listenForConnections();
		CommandWorker::storeCommands();
	}
	catch(const std::exception& e)
	{
		perror(e.what());
		exit(EXIT_FAILURE);
	}
}

void	App::run(void)
{
	int			res;
	bool		keepAlive;

	keepAlive = true;
	this->server.pfds.push_back(this->server.initPollFd(this->server.getMasterSocketFd(), POLLIN, 0));
	while (keepAlive)
	{
		res = -1;
		res = poll(&this->server.pfds[0], this->server.pfds.size(), 0);
		if (res < 0)
			throw std::runtime_error("poll() failed");
		if (this->server.pfds[0].revents & POLLIN)
			this->server.acceptConnections();
		for (size_t i = 1; i < this->server.pfds.size() && res; i++)
		{
			if (this->server.pfds[i].revents & POLLIN)
			{
				int	requestResult = 0;
				requestResult = this->server.readRequest(this->server.clients.at(this->server.pfds[i].fd));
				if (requestResult <= 0)
					this->server.clean(this->server.pfds[i].fd);
				res--;
			}
		}
	}
}
