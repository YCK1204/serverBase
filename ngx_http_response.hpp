/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ngx_http_response.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelee <kelee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/31 07:52:51 by kelee             #+#    #+#             */
/*   Updated: 2023/06/04 17:00:51 by kelee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NGX_HTTP_RESPONSE
# define NGX_HTTP_RESPONSE

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <iterator>

#include "ngx_http_common.hpp"
#include "ngx_http_request.hpp"
#include <dirent.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <map>
#include <vector>


/**
    The response code line has the form

    <version> <responsecode> <responsestring>

    where <version> is the protocol version ("HTTP/1.0" or "HTTP/1.1"),
    <responsecode> is a 3-digit response code indicating success or
    failure of the request, and <responsestring> is an optional
    human-readable string explaining what the response code means.
 
*/

class HTTPResponse : public NgxHttpMethod, public NgxHttpStatus, public MIMETypes {
    public:
        /*
        # Header name/value ABNF (http://tools.ietf.org/html/rfc7230#section-3.2)
        #
        # VCHAR          = %x21-7E
        # obs-text       = %x80-FF
        # header-field   = field-name ":" OWS field-value OWS
        # field-name     = token
        # field-value    = *( field-content / obs-fold )
        # field-content  = field-vchar [ 1*( SP / HTAB ) field-vchar ]
        # field-vchar    = VCHAR / obs-text
        #
        # obs-fold       = CRLF 1*( SP / HTAB )
        #                ; obsolete line folding
        #                ; see Section 3.2.4

        # token          = 1*tchar
        #
        # tchar          = "!" / "#" / "$" / "%" / "&" / "'" / "*"
        #                / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~"
        #                / DIGIT / ALPHA
        #                ; any VCHAR, except delimiters
        #
        # VCHAR defined in http://tools.ietf.org/html/rfc5234#appendix-B.1
        */
       
        HTTPResponse(int& clientSock,  const BaseHTTPRequest& objRequest) {

            statusCode = "000";
            Connection = "";
            ContentType = "";
            body = "";
            
            switch (methodTbl[objRequest.getMethod()]) {
                case NGX_HTTP_GET       :   statusCode = httpGET(objRequest.getTarget());    break;     // READ
                // case NGX_HTTP_HEAD      :   statusCode = httpHEAD(objRequest.getTarget());   break;
                case NGX_HTTP_POST      :   statusCode = httpPOST();   break;                           // CREATE
                // case NGX_HTTP_PUT       :   statusCode = httpPUT();    break;                           // UPDATE
                case NGX_HTTP_DELETE    :   statusCode = httpDELETE(); break;                           // DELETE
                // case NGX_HTTP_CONNECT   :   httpCONNECT();break;
                // case NGX_HTTP_OPTIONS   :   httpOPTIONS();break;
                // case NGX_HTTP_TRACE     :   httpTRACE();  break;
                default :
                    break;
            }
            
            sendResponseMessage(clientSock, objRequest.getMethod(), statusCode, body, ContentType);
        }
        virtual ~HTTPResponse() {}

        /*  orig-date    = "Date:" date-time CRLF https://datatracker.ietf.org/doc/html/rfc5322#section-3.6.1
            HTTP-date    = IMF-fixdate / obs-date https://datatracker.ietf.org/doc/html/rfc7231#section-7.1.1.1
            Date: Tue, 15 Nov 1994 08:12:31 GMT
        */
        static const std::string&  getDateHTTPformat() {
            std::time_t rawtime;     std::time(&rawtime);
            std::tm*    timeinfo;    timeinfo = std::gmtime(&rawtime);
            
            char buffer[128];        std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
            return ("Date: " + std::string(buffer));
        }

        static void sendResponseMessage(int& clientSock, 
                                        const std::string& method,
                                        const std::string& statusCode, 
                                        const std::string& body = std::string(""), 
                                        const std::string& contentType = DEFAULT_ERROR_CONTENT_TYPE,
                                        const std::vector<std::string>& additionalHeader = std::vector<std::string>()) {
            
            std::string statusLine = HTTPResponse::version + statusCode + statusTbl[statusCode];
            std::string Date = getDateHTTPformat();
            std::string server = HTTPResponse::Server;
            // Connection ;
            std::string ContentType = "Content-Type: " + contentType;

            std::string modifiedBody = body;
            if (statusCode == "404")                                                                                                {modifiedBody = DEFAULT_ERROR_MESSAGE;}
            std::string ContentLength = "Content-Length: " + std::to_string(modifiedBody.length());
            if (method == "HEAD")                                                                                                   {modifiedBody = ""; }

            std::stringstream oneLine;
            oneLine << statusLine << CRLF;
            oneLine << Date << CRLF;
            oneLine << server << CRLF;
            oneLine << ContentType << CRLF;
            oneLine << ContentLength << CRLF;

            for (std::vector<std::string>::const_iterator it = additionalHeader.begin(); it != additionalHeader.end(); it++) {
                oneLine << *it << CRLF;
            }

            oneLine << CRLF;

            oneLine << modifiedBody << CRLF;

            send(clientSock, oneLine.str().c_str(), oneLine.str().length(), 0);
        }
        
    private:

        // const std::string& translate_path(const std::string & targetURI) {
        //     // abandon query parameters
        //     path = path.split('?',1)[0]
        //     path = path.split('#',1)[0]
        // }

        const std::string& listDirectory(std::string& path) {
            std::vector<std::string> files;
            DIR* directory = opendir(path.c_str());
            if (directory != NULL) {
                struct dirent* entry;
                while ((entry = readdir(directory)) != NULL) {
                    // if (entry->d_type == DT_REG)   // 일반 파일인지 확인
                    files.push_back(entry->d_name);
                }
                closedir(directory);
            }
            else
                return ("403");
            std::sort(files.begin(), files.end());
            
            std::stringstream html;
            html << "<!DOCTYPE HTML>\n";
            html << "<html lang=\"en\">\n";
            html << "<head>\n";
            html << "<meta charset=\"UTF-8\">\n";
            html << "<title>Directory listing for " << path << "</title>\n";
            html << "</head>\n";
            html << "<body>\n";
            html << "<h1>Directory listing for " << path << "</h1>\n";
            html << "<hr>\n";
            html << "<ul>\n";

            for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); it++) {
                std::string fullPath = path + "/" + *it;
                std::string linkName = escapeHTML(*it);
                std::string displayName = linkName;
                /* Append / for directories or @ for symbolic links */
                struct stat fileStat;
                if (stat(fullPath.c_str(), &fileStat) == 0){
                    if      (S_ISDIR(fileStat.st_mode))              {displayName = *it + "/"; linkName = *it + "/"; }
                    else if (S_ISLNK(fileStat.st_mode))              {linkName = *it + "@"; }
                }
                std::string escapedLinkname = escapeHTML(std::filesystem::weakly_canonical(fullPath).string());
                html << "<li><a href=\"" << escapedLinkname << "\">" << displayName << "</a></li>\n";
            }
            html << "</ul>\n";
            html << "<hr>\n";
            html << "</body>\n";
            html << "</html>\n";

            ContentType = "Content-Type: " + "text/html" + ";charset=utf-8";
            body = html.str();
            return ("200");
        }

        /**
         * 200 OK: 요청이 성공하였고, 요청한 리소스가 성공적으로 반환됨                      [v]
         * 204 No Content: 요청이 성공적으로 처리되었지만, 반환할 콘텐츠가 없음을 나타냅니다. 예를 들어, 검색 결과가 없는 경우 등에 사용됩니다.
         * 400 Bad Request: 잘못된 요청으로 인해 서버가 요청을 이해할 수 없음               [v]
         * 401 Unauthorized: 요청한 리소스에 접근하기 위해 인증이 필요함
         * 403 Forbidden: 요청한 리소스에 대한 접근이 거부됨
         * 404 Not Found: 요청한 리소스가 서버에 존재하지 않음                           [v]
         * 405 Method Not Allowed: 요청한 HTTP 메서드(GET, POST 등)가 허용되지 않음을 나타냅니다. 예를 들어, 읽기 전용 자원에 POST 요청을 보낸 경우 등에 사용됩니다. 
         * 500 Internal Server Error: 서버에서 요청을 처리하는 동안 내부적인 오류가 발생함     
        */
        const std::string& httpGET(const std::string& path) {
            /**
                A client can alter the semantics of GET to be a "range request",
                requesting transfer of only some part(s) of the selected
                representation, by sending a Range header field in the request
                ([RFC7233]).
                
                The response to a GET request is cacheable; a cache MAY use it to
                satisfy subsequent GET and HEAD requests unless otherwise indicated
                by the Cache-Control header field (Section 5.2 of [RFC7234]).

                might lead some implementations to reject the request and close the connection
                because of its potential as a request smuggling attack (Section 11.2 of [HTTP/1.1]
            */
           
            // std::string path = translate_path(target); // cgi 
            struct stat fileStat;
            if (stat(path.c_str(), &fileStat) < 0)                                                                                    { return ("404"); }
            
            if (S_ISDIR(fileStat.st_mode)) {
                std::string newPath = path;
                /* redirect browser - doing basically what apache does */
                if (newPath.back() != '/')                              { newPath += '/'; addHeader.push_back("Location: " + newPath); return ("301"); }
                
                const char* indexfile[2] = {"index.html", "index.htm"};
                for (int i = 0; i < 2 ; i++) {
                    std::string indexpath = path + std::string(indexfile[i]);
                    if (stat(indexpath.c_str(), &fileStat) == 0)                                    { newPath = indexpath; break ; }
                }
                if (newPath == path)
                    return listDirectory(newPath);
            }

            std::string suffix = path.substr(path.rfind(".") + 1);
            std::map<std::string, std::string>::const_iterator it = MIMETypeTbl.find(suffix);       if (it == MIMETypeTbl.end())     { return ("404"); }
            ContentType = "Content-Type: " + it->second + ";charset=utf-8";

            std::ifstream file(path.c_str(), std::ios::binary);                                     if (!file.is_open())             { return ("404"); }
            char buffer[4096];
            while (file.good())                                                                     { file.read(buffer, sizeof(buffer)); body += std::string(buffer); }
            file.close();
            return ("200");
        }
        
        /* The HEAD method is identical to GET except that the server MUST NOT send content in the response */
        const std::string& httpHEAD(const std::string& path)                                        { return (httpGET(path)); }
    
        /* POST request, the origin server SHOULD send a 201 (Created) response containing a Location header field that provides an identifier for the primary resource created  */
        void httpPOST() {
            /**
             * 
             * POST /test HTTP/1.1
                Host: foo.example
                Content-Type: application/x-www-form-urlencoded
                Content-Length: 27

                field1=value1&field2=value2

            */

            /**
            POST /test HTTP/1.1
            Host: foo.example
            Content-Type: multipart/form-data;boundary="boundary"
            
            --boundary
            Content-Disposition: form-data; name="field1"
            
            value1
            --boundary
            Content-Disposition: form-data; name="field2"; filename="example.txt"
            
            value2
            --boundary--

            */
           
        }
        /* 200 UPDATE, 201 CREATE */
        void httpPUT() {
            // HTTP/1.1 201 Created
            // Content-Location: /new.html


            // HTTP/1.1 204 No Content
            // Content-Location: /existing.html

        }
        void httpDELETE() {
            // A 202 (Accepted) status code if the action will likely succeed but has not yet been enacted.
            // A 204 (No Content) status code if the action has been enacted and no further information is to be supplied.
            // A 200 (OK) status code if the action has been enacted and the response message includes a representation describing the status.
        }
        // void httpCONNECT() {
            
        // }
        // void httpOPTIONS() {
            
        // }
        // void httpTRACE() {
            
        // }

        /* statusLine */
        const static std::string version = std::string("HTTP/1.1 ");
        std::string statusCode;
        /* Response HEADER */
        const static std::string Server = "Server: WebServer/1.0.0 (42Seoul)";
        std::string              Connection;
        std::string              ContentType;
        std::vector<std::string> addHeader;
        /* body */
        std::string body;
        
        const static long maxline = 65536;
        const static int maxheaders = 100;
};