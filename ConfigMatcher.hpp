/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigMatcher.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thodavid <thodavid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 11:50:44 by thodavid          #+#    #+#             */
/*   Updated: 2026/06/02 14:20:51 by thodavid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "http/RequestContext.hpp"
#include "config/ServerConfig.hpp"
#include <cstddef>
#include <string>
#include <vector>

struct RouteMatch
{
    const ServerConfig *server;
    const Location *location;
};

struct EffectiveRoute
{
    const ServerConfig *server;
    const Location *location;

    std::string root;
    std::string upload_store;
    std::string index;
    std::vector<std::string> allow_methods;
    size_t client_max_body_size;
    size_t client_max_header_size;
    bool autoindex;

    int return_code;
    std::string return_url;

    std::map<std::string, std::string> cgi;
};

class ConfigMatcher
{
public:
    RouteMatch match(const RequestContext &ctx,
                     const std::vector<ServerConfig> &servers) const;

private:
    const ServerConfig *findServer(const RequestContext &ctx,
                                   const std::vector<ServerConfig> &servers) const;

    const Location *findLocation(const RequestContext &ctx,
                                 const ServerConfig &server) const;

    bool serverNameMatch(const RequestContext &ctx,
                         const ServerConfig &server) const;

    bool portMatch(const RequestContext &ctx,
                   const ServerConfig &server) const;

    bool locationMatch(const std::string &request_path,
                       const std::string &location_path) const;
};