/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ngx_http_request.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelee <kelee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/31 07:52:51 by kelee             #+#    #+#             */
/*   Updated: 2023/06/04 15:59:27 by kelee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NGX_HTTP_REQUEST
# define NGX_HTTP_REQUEST

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <iterator>

#include "ngx_http_common.hpp"
#include "ngx_http_response.hpp"
#include <map>
#include <vector>

std::vector<std::string> split(const std::string &line, char delimiter) {
    std::istringstream       iss(line);
    std::string              buf;
    std::vector<std::string> tokens;
    while (std::getline(iss, buf, delimiter)) 
        tokens.push_back(buf);
    return (tokens);
}

/* 
    HTTP Request classes.

    - BaseHTTPRequestHandler doesn't implement any HTTP request; 
    - SimpleHTTPRequestHandler for simple implementations of GET, HEAD and POST,
    - CGIHTTPRequestHandler for CGI scripts.
        - GET and POST requests to cgi-bin scripts.
        - the status code 200 is sent prior to execution of a CGI script
        - so scripts cannot send other status codes such as 302 (redirect).

    It does, however, optionally implement HTTP/1.1 persistent connections,
    as of version 0.3.

TODO:
- log requests even later (to capture byte count)
- log user-agent header and other interesting goodies
- send error log to separate file
*/

/*
 See also:

 HTTP Working Group                                        T. Berners-Lee
 INTERNET-DRAFT                                            R. T. Fielding
 <draft-ietf-http-v10-spec-00.txt>                     H. Frystyk Nielsen
 Expires September 8, 1995                                  March 8, 1995

 URL: http://www.ics.uci.edu/pub/ietf/http/draft-ietf-http-v10-spec-00.txt

 and

 Network Working Group                                      R. Fielding
 Request for Comments: 2616                                       et al
 Obsoletes: 2068                                              June 1999
 Category: Standards Track

 URL: http://www.faqs.org/rfcs/rfc2616.html
 Log files
 ---------

 Here's a quote from the NCSA httpd docs about log file format.

 | The logfile format is as follows. Each line consists of:
 |
 | host rfc931 authuser [DD/Mon/YYYY:hh:mm:ss] "request" ddd bbbb
 |
 |        host: Either the DNS name or the IP number of the remote client
 |        rfc931: Any information returned by identd for this person,
 |                - otherwise.
 |        authuser: If user sent a userid for authentication, the user name,
 |                  - otherwise.
 |        DD: Day
 |        Mon: Month (calendar name)
 |        YYYY: Year
 |        hh: hour (24-hour format, the machine's timezone)
 |        mm: minutes
 |        ss: seconds
 |        request: The first line of the HTTP request as sent by the client.
 |        ddd: the status code returned by the server, - if not available.
 |        bbbb: the total number of bytes sent,
 |              *not including the HTTP/1.0 header*, - if not available
 |
 | You can determine the name of the file accessed through request.
*/


class HTTPResponse;

class BaseHTTPRequest : public NgxHttpStatus {

    /*
    HTTP request handler base class.

    HTTP (HyperText Transfer Protocol) is an extensible protocol on
    top of a reliable stream transport (e.g. TCP/IP).  The protocol
    recognizes three parts to a request:

    1. One line identifying the request type and path
    2. An optional set of RFC-822-style headers
    3. An optional data part

    The headers and data are separated by a blank line.

    The first line of the request has the form

    <command> <path> <version>
    - <command> is a (case-sensitive) keyword such as GET or POST
    - <path> is a string containing path information for the request
    - <version> should be the string "HTTP/1.0" or "HTTP/1.1".

    <path> is encoded using the URL encoding scheme (using %xx to signify
    the ASCII character with hex code xx).

    The specification specifies that lines are separated by CRLF but
    for compatibility with the widest range of clients recommends
    servers also handle LF.  Similarly, whitespace in the request line
    is treated sensibly (allowing multiple spaces between components
    and allowing trailing whitespace).

    Similarly, for output, lines ought to be separated by CRLF pairs
    but most clients grok LF characters just fine.

    If the first line of the request has the form

    <command> <path>

    (i.e. <version> is left out) then this is assumed to be an HTTP
    0.9 request; this form has no optional headers and data part and
    the reply consists of just the data.

    The reply form of the HTTP 1.x protocol again has three parts:

    1. One line giving the response code
    2. An optional set of RFC-822-style headers
    3. The data

    Again, the headers and data are separated by a blank line.

    This server parses the request and the headers, and then calls a
    function specific to the request type (<command>).  Specifically,
    a request SPAM will be handled by a method do_SPAM().  If no
    such method exists the server sends an error response to the
    client.  If it exists, it is called with no arguments:

    ------------------------------------------------------------
    
    The various request details are stored in instance variables:

    - client_address is the client IP address in the form (host,
    port);

    - command, path and version are the broken-down request line;

    - headers is an instance of email.message.Message (or a derived
    class) containing the header information;

    - rfile is a file object open for reading positioned at the
    start of the optional input data part;

    - wfile is a file object open for writing.

    IT IS IMPORTANT TO ADHERE TO THE PROTOCOL FOR WRITING!

    The first thing to be written must be the response line.  Then
    follow 0 or more header lines, then a blank line, and then the
    actual data (if any).  The meaning of the header lines depends on
    the command executed by the server; in most cases, when data is
    returned, there should be at least one header line of the form

    Content-type: <type>/<subtype>

    where <type> and <subtype> should be registered MIME types,
    e.g. "text/html" or "text/plain".

    */

    public :
        BaseHTTPRequest(int& clientSock, std::string& rawRequestline) throw(std::invalid_argument) {
            method = "";
            target = "";
            version = "HTTP/0.9";
            close_connection = true;
            body = "";
            std::stringstream ss(rawRequestline);

            try {
                
                std::size_t pos = rawRequestline.find(CRLF);                                    if (pos == std::string::npos)                                                                  { throw std::invalid_argument("Bad request"); }
                parse_requestLine(rawRequestline.substr(0, pos));
                rawRequestline = rawRequestline[pos + 1];

                std::size_t pos = rawRequestline.find(CRLFCRLF);                                if (pos == std::string::npos)                                                                  { throw std::invalid_argument("Bad request");}
                parse_headers(rawRequestline.substr(0, pos));
                rawRequestline = rawRequestline[pos + 1];

                parse_body(rawRequestline, clientSock);
                connection_control();
            } catch(std::invalid_argument& e) { HTTPResponse::sendResponseMessage(clientSock, method, "400", e.what());
            } catch(std::out_of_range &e)     { HTTPResponse::sendResponseMessage(clientSock, method, "505", e.what());
            } catch(std::overflow_error &e)   { HTTPResponse::sendResponseMessage(clientSock, method, "431", e.what());
            }
        }
        virtual ~BaseHTTPRequest()                                                              {}
    
        void parse_requestLine(const std::string& requestLine) throw(std::invalid_argument, std::out_of_range) {
            std::vector<std::string> tokens = split(requestLine, ' ');
            int                      token_size = tokens.size();                                if (!(token_size == 2 || token_size == 3))                                                      { throw std::invalid_argument("Bad request " + requestLine); }

            version = tokens[2];                                                                if (version.compare(0, 5, "HTTP/") != 0)                                                        { throw std::invalid_argument("Invalid HTTP version"); }

            std::string base_version_number = version.substr(5);
            std::size_t dot_pos = base_version_number.find('.');                                if (dot_pos == std::string::npos)                                                               { throw std::invalid_argument("Invalid HTTP version"); }

            std::string major_str = base_version_number.substr(0, dot_pos);
            std::string minor_str = base_version_number.substr(dot_pos + 1);
                                                                                                if (major_str.empty() || minor_str.empty() || major_str.size() > 10 || minor_str.size() > 10)   { throw std::invalid_argument("Invalid HTTP version"); }
                                                                                                if (minor_str.find('.') != std::string::npos)                                                   { throw std::invalid_argument("Invalid HTTP version"); }
                                                                                                /* RFC 2145 section 3.1 says there can be only one "." and major and minor numbers MUST be treated as separate integers */
            int major = std::stoi(major_str);
            int minor = std::stoi(minor_str);
                                                                                                if (major >= 2 &&  minor >= 0)                                                                  { throw std::out_of_range("Invalid HTTP version" + base_version_number); }
                                                                                                if (major >= 1 &&  minor >= 1)                                                                  { close_connection = false;}
            target = tokens[1];
            method = tokens[0];                                                                 if (token_size == 2 && method != "GET")                                                         { throw std::invalid_argument("Bad HTTP/0.9 request type" + requestLine); }
        }

        void parse_headers(const std::string& requestHeader) throw(std::invalid_argument, std::overflow_error) {

            std::vector<std::string> tokens = split(requestHeader, LF);                         if (tokens.size() > maxheaders)                                                                 { throw std::overflow_error("Too many headers"); }

            for (std::vector<std::string>::iterator it = tokens.begin() ; it != tokens.end() ; it++) {
                std::string headerStr = *it;                                                    if (headerStr.length() > maxline)                                                               { throw std::overflow_error("Line too long");}

                std::size_t endPos = headerStr.find("\r");                                      if (endPos == std::string::npos)                                                                { throw std::invalid_argument("invalid HTTP header line"); }
                headerStr[endPos] = '\0';
                
                std::size_t colon_pos = headerStr.find(":");                                    if (colon_pos == std::string::npos)                                                             { throw std::invalid_argument("invalid HTTP header line"); }
                
                std::string key = headerStr.substr(0, colon_pos);
                std::string value = headerStr.substr(colon_pos + 1);                            if (value.length() >= 1 && value[0] == ' ')                                                     {value = value.substr(1);}
                std::map<std::string,std::string>::iterator dup_key = headers.find(key);        if (dup_key != headers.end())                                                                   { throw std::invalid_argument("invalid HTTP header line"); }
                headers[key] = value;
            }
            std::map<std::string,std::string>::iterator it = headers.find("Host");              if (it == headers.end())                                                                        { throw std::invalid_argument("invalid HTTP header line"); }
        }

        void     parse_body(const std::string & body, int& clientSock) { 

            if (headers["User-Agent"].find("Safari/605.1.15") != std::string::npos) {
                char buf[1024]; size_t bytes_read = recv(clientSock, buf, sizeof(buf), 0);
                this->body = std::string(buf);
            } else {
                this->body = body; 
            }
        }

        void    connection_control() {
            std::string conntype = headers["Connection"];
            for (std::string::iterator it = conntype.begin(); it != conntype.end(); it ++)
                tolower(*it);
            if (conntype == "close")
                close_connection = true;                
        }
        const std::string&  getMethod() const                                                   { return (method); }
        const std::string&  getTarget() const                                                   { return (target); }


    private:
        std::string                         method;
        std::string                         target;
        std::string                         version;

        std::map<std::string, std::string>  headers;
        std::string                         body;

        bool  close_connection;

        const static long maxline = 65536;
        const static int maxheaders = 100;
};
#endif