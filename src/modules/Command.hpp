/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sel-kham <sel-kham@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/28 00:51:57 by sel-kham          #+#    #+#             */
/*   Updated: 2023/08/31 01:31:28 by sel-kham         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef __COMMAND_HPP__
# define __COMMAND_HPP__

# include <map>
# include "Globals.hpp"

class Command
{
protected:
	/* Private class attributes */
	str_t	request;
	str_t	command;
	str_t	response;
public:
	/* Constructors & Destructors */
	Command(void);
	~Command(void);

	/* Getters & Setters */
	const str_t	&getRequest(void) const;
	void		setRequest(const str_t &request);
	const str_t	&getCommand(void) const;
	void		setCommand(const str_t &command);
	const str_t	&getResponse(void) const;
	void		setResponse(const str_t &response);
	
	/* Class  functionallities */
	void	extractCommand(void);
};

#endif