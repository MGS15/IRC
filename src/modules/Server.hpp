/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sel-kham <sel-kham@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/27 01:34:15 by sel-kham          #+#    #+#             */
/*   Updated: 2023/08/30 00:31:53 by sel-kham         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef __SERVER_HP__
# define __SERVER_HP__

# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <fcntl.h>
# include <poll.h>
# include <unistd.h>
# include <vector>
# include <map>
# include "Globals.hpp"
# include "Client.hpp"

typedef struct pollfd				pollfd_t;
typedef std::vector<pollfd_t>		pollfd_v;
typedef struct sockaddr_in			sockaddr_in_t;
typedef std::map<const int, Client>	client_m;

class Server
{
private:
	/* Class Attributes */
	int				masterSocketFd;
	int				port;
	str_t			password;
	sockaddr_in_t	addr;
	str_t			host;
public:
	/* Public Class Attributes */
	pollfd_v		pfds;
	client_m		clients;
	/* Constructors & Destructors */
	Server(void);
	Server(const int &port, const str_t &password);
	~Server(void);

	/* Getters & Setters */
	const int	&getMasterSocketFd(void) const;
	void		setMasterSocketFd(const int &fd);
	const int	&getPort(void) const;
	void		setPort(const int &port);
	const str_t	&getPassword(void) const;
	void		setPassword(const str_t &pass);
	const str_t	&getHost(void) const;

	/* External Class functionalities */
	void		initSocketMaster(void);
	void		bindSocketMaster(void);
	void		listenForConnections(void);
	int			acceptConnections(Client &client);
	const str_t	welcomeMessage(const Client &client);
	pollfd_t	initPollFd(int fd, short event, short revent);
	int			readRequest(Client &client);
	void		integrateNewConnect(Client &client);
	void		clean(const int &index);
	void		getServerHost(void);
};

#endif