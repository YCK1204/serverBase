/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ngx_http_common.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelee <kelee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/31 07:24:48 by kelee             #+#    #+#             */
/*   Updated: 2023/06/04 14:36:24 by kelee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NGX_HTTP_COMMON
# define NGX_HTTP_COMMON


#include <iostream>
#include <string>
#include <map>

#define LF                          '\n'
#define CR                          '\r'
#define CRLF                        "\r\n"
#define CRLFCRLF                    "\r\n\r\n"


#define DEFAULT_ERROR_CONTENT_TYPE  "text/html;charset=utf-8"
#define DEFAULT_ERROR_MESSAGE       "                               \
<!DOCTYPE HTML>                                                     \
<html lang=\"en\">                                                  \
    <head>                                                          \
        <meta charset=\"utf-8\">                                    \
        <title>Error response</title>                               \
    </head>                                                         \
    <body>                                                          \
        <center><h1>404 Not Found</h1></center>                     \
    </body>                                                         \
</html>                                                             \
"

class NgxHttpMethod {
    /*  HTTP methods and descriptions
     *  Methods from the following RFCs are all observed:
     *  * RFC 7231: Hypertext Transfer Protocol (HTTP/1.1), obsoletes 2616
     *  * RFC 5789: PATCH Method for HTTP
    */
   public:
        NgxHttpMethod() {

            methodTbl["GET"]       = NGX_HTTP_GET;           // "Retrieve the target.";
            methodTbl["HEAD"]      = NGX_HTTP_HEAD;          // "Same as GET, but only retrieve the status line and header section.";
            methodTbl["POST"]      = NGX_HTTP_POST;          // "POST', 'Perform target-specific processing with the request payload.";
            methodTbl["PUT"]       = NGX_HTTP_PUT;           // "Replace the target with the request payload.";
            methodTbl["DELETE"]    = NGX_HTTP_DELETE;        // "Remove the target.";
            methodTbl["CONNECT"]   = NGX_HTTP_CONNECT;       // "Establish a connection to the server.";
            methodTbl["OPTIONS"]   = NGX_HTTP_OPTIONS;       // "Describe the communication options for the target.";
            methodTbl["TRACE"]     = NGX_HTTP_TRACE;         // "Perform a message loop-back test along the path to the target.";
            // NGX_HTTP_PROPFIND
            // NGX_HTTP_MKCOL
            // NGX_HTTP_COPY
            // NGX_HTTP_MOVE
            // NGX_HTTP_PROPPATCH
            // NGX_HTTP_LOCK
            // NGX_HTTP_UNLOCK
            // NGX_HTTP_PATCH
        }
        virtual ~NgxHttpMethod()  {}

    protected:
        std::map<std::string, int>  methodTbl;
        enum NGX_HTTP_METHOD {
            NGX_HTTP_UNKNOWN,
            NGX_HTTP_GET,
            NGX_HTTP_HEAD,
            NGX_HTTP_POST,
            NGX_HTTP_PUT,
            NGX_HTTP_DELETE,
            NGX_HTTP_CONNECT,
            NGX_HTTP_OPTIONS,
            NGX_HTTP_TRACE
        };
};

class NgxHttpStatus {
    /*  RFC 2616 , 1999
        RFC 7231 , 2014
        RFC 9112 , 2022 
    */
    /*
     * HTTP status codes and reason phrases
     * Status codes from the following RFCs are all observed:
     *     * RFC 7231: Hypertext Transfer Protocol (HTTP/1.1), obsoletes 2616
     *     * RFC 6585: Additional HTTP Status Codes
     *     * RFC 3229: Delta encoding in HTTP
     *     * RFC 4918: HTTP Extensions for WebDAV, obsoletes 2518
     *     * RFC 5842: Binding Extensions to WebDAV
     *     * RFC 7238: Permanent Redirect
     *     * RFC 2295: Transparent Content Negotiation in HTTP
     *     * RFC 2774: An HTTP Extension Framework
     *     * RFC 7725: An HTTP Status Code to Report Legal Obstacles
     *     * RFC 7540: Hypertext Transfer Protocol Version 2 (HTTP/2)
     *     * RFC 2324: Hyper Text Coffee Pot Control Protocol (HTCPCP/1.0)
     *     * RFC 8297: An HTTP Status Code for Indicating Hints
     *     * RFC 8470: Using Early Data in HTTP
    */
   public:
        NgxHttpStatus() {
            /* informational */
            statusTbl["100"] = "Continue";
            statusTbl["101"] = "Switching Protocols";
            statusTbl["102"] = "Processing";
            statusTbl["103"] = "Early Hints";

            /* success */
            statusTbl["200"] = "OK";
            statusTbl["201"] = "Created";
            statusTbl["202"] = "Accepted";
            statusTbl["203"] = "Non-Authoritative Information";
            statusTbl["204"] = "No Content";
            statusTbl["205"] = "Reset Content";
            statusTbl["206"] = "Partial Content";
            statusTbl["207"] = "Multi-Status";
            statusTbl["208"] = "Already Reported";
            statusTbl["226"] = "IM Used";
            
            /* redirection */
            statusTbl["300"] = "Multiple Choices";
            statusTbl["301"] = "Moved Permanently";
            statusTbl["302"] = "Found";                // MOVED_TEMPORARILY 
            statusTbl["303"] = "See Other";
            statusTbl["304"] = "Not Modified";
            statusTbl["305"] = "Use Proxy";
            statusTbl["307"] = "Temporary Redirect";
            statusTbl["308"] = "Permanent Redirect";

            /* client error */
            statusTbl["400"] = "Bad Request";
            statusTbl["401"] = "Unauthorized";
            statusTbl["402"] = "Payment Required";
            statusTbl["403"] = "Forbidden";
            statusTbl["404"] = "Not Found";
            statusTbl["405"] = "Method Not Allowed";
            statusTbl["406"] = "Not Acceptable";
            statusTbl["407"] = "Proxy Authentication Required";
            statusTbl["408"] = "Request Timeout";
            statusTbl["409"] = "Conflict";
            statusTbl["410"] = "Gone";
            statusTbl["411"] = "Length Required";
            statusTbl["412"] = "Precondition Failed";
            statusTbl["413"] = "Request Entity Too Large";
            statusTbl["414"] = "Request-URI Too Long";
            statusTbl["415"] = "Unsupported Media Type";
            statusTbl["416"] = "Requested Range Not Satisfiable";
            statusTbl["417"] = "Expectation Failed";
            // statusTbl["418"] = "I\'m a Teapot";
            // statusTbl["421"] = "Misdirected Request";
            // statusTbl["422"] = "Unprocessable Entity";
            // statusTbl["423"] = "Locked";
            // statusTbl["424"] = "Failed Dependency";
            // statusTbl["425"] = "Too Early";
            statusTbl["426"] = "Upgrade Required";
            // statusTbl["428"] = "Precondition Required";
            // statusTbl["429"] = "Too Many Requests";
            statusTbl["431"] = "Request Header Fields Too Large";
            // statusTbl["451"] = "Unavailable For Legal Reasons";fgetStatusPhrase
    
            /* server errors */
            statusTbl["500"] = "Internal Server Error";
            statusTbl["501"] = "Not Implemented";
            statusTbl["502"] = "Bad Gateway";
            statusTbl["503"] = "Service Unavailable";
            statusTbl["504"] = "Gateway Timeout";
            statusTbl["505"] = "HTTP Version Not Supported";
            // statusTbl["506"] = "Variant Also Negotiates";
            // statusTbl["507"] = "Insufficient Storage";
            // statusTbl["508"] = "Loop Detected";
            // statusTbl["510"] = "Not Extended";
            // statusTbl["511"] = "Network Authentication Required";
        }

        virtual ~NgxHttpStatus() {}
        
        // static const std::string& getStatusPhrase(const std::string& statusCode) const {
        //     std::map<std::string, std::string>::const_iterator it = statusTbl.find(statusCode);
        //     if (it != statusTbl.end()) {
        //         return it->second;
        //     } else {
        //         // 키를 찾지 못한 경우에 대한 예외 처리
        //         // 예를 들어, 기본 상태 구문을 반환하거나 예외를 throw할 수 있습니다.
        //         return "Unknown Status";
        //     }
        // }
   protected:
        static std::map<std::string , std::string> statusTbl;
};

class MIMETypes {
    public:
        MIMETypes() {
            // text
            MIMETypeTbl["html"]     = "text/html";
            MIMETypeTbl["htm"]      = "text/html";
            MIMETypeTbl["shtml"]    = "text/html";
            MIMETypeTbl["css"]      = "text/css";
            MIMETypeTbl["xml"]      = "text/xml";
            MIMETypeTbl["txt"]      = "text/plain";
            MIMETypeTbl["jad"]      = "text/vnd.sun.j2me.app-descriptor";
            MIMETypeTbl["wml"]      = "text/vnd.wap.wml";
            MIMETypeTbl["htc"]      = "text/x-component";
            MIMETypeTbl["mml"]      = "text/mathml";
            // image
            MIMETypeTbl["gif"] = "image/gif";
            MIMETypeTbl["jpeg"] = "image/jpeg";
            MIMETypeTbl["jpg"] = "image/jpeg";
            MIMETypeTbl["png"] = "image/png";
            MIMETypeTbl["svg"] = "image/svg+xml";
            MIMETypeTbl["svgz"] = "image/svg+xml";
            MIMETypeTbl["tiff"] = "image/tiff";
            MIMETypeTbl["tif"] = "image/tiff";
            MIMETypeTbl["wbmp"] = "image/vnd.wap.wbmp";
            MIMETypeTbl["webp"] = "image/webp";
            MIMETypeTbl["ico"] = "image/x-icon";
            MIMETypeTbl["jng"] = "image/x-jng";
            MIMETypeTbl["bmp"] = "image/x-ms-bmp";
            // font
            MIMETypeTbl["woff"] = "font/woff";
            MIMETypeTbl["woff2"] = "font/woff2";
            // application
            MIMETypeTbl["js"] = "application/javascript";
            MIMETypeTbl["atom"] = "application/atom+xml";
            MIMETypeTbl["rss"] = "application/rss+xml";
            MIMETypeTbl["json"] = "application/json";
            MIMETypeTbl["jar"] = "application/java-archive";
            MIMETypeTbl["war"] = "application/java-archive";
            MIMETypeTbl["ear"] = "application/java-archive";
            MIMETypeTbl["pdf"] = "application/pdf";
            MIMETypeTbl["doc"] = "application/msword";
            MIMETypeTbl["xls"] = "application/vnd.ms-excel";
            MIMETypeTbl["ppt"] = "application/vnd.ms-powerpoint";
            MIMETypeTbl["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
            MIMETypeTbl["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
            MIMETypeTbl["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
            MIMETypeTbl["wasm"] = "application/wasm";
            MIMETypeTbl["zip"] = "application/zip";
            MIMETypeTbl["7z"] = "application/x-7z-compressed";
            MIMETypeTbl["cco"] = "application/x-cocoa";
            MIMETypeTbl["jardiff"] = "application/x-java-archive-diff";
            MIMETypeTbl["jnlp"] = "application/x-java-jnlp-file";
            MIMETypeTbl["run"] = "application/x-makeself";
            MIMETypeTbl["pl"] = "application/x-perl";
            MIMETypeTbl["pm"] = "application/x-perl";
            MIMETypeTbl["prc"] = "application/x-pilot";
            MIMETypeTbl["pdb"] = "application/x-pilot";
            MIMETypeTbl["rar"] = "application/x-rar-compressed";
            MIMETypeTbl["rpm"] = "application/x-redhat-package-manager";
            MIMETypeTbl["sea"] = "application/x-sea";
            MIMETypeTbl["swf"] = "application/x-shockwave-flash";
            MIMETypeTbl["sit"] = "application/x-stuffit";
            MIMETypeTbl["tcl"] = "application/x-tcl";
            MIMETypeTbl["tk"] = "application/x-tcl";
            MIMETypeTbl["der"] = "application/x-x509-ca-cert";
            MIMETypeTbl["pem"] = "application/x-x509-ca-cert";
            MIMETypeTbl["crt"] = "application/x-x509-ca-cert";
            MIMETypeTbl["xpi"] = "application/x-xpinstall";
            MIMETypeTbl["der"] = "application/x-x509-ca-cert";
            MIMETypeTbl["pem"] = "application/x-x509-ca-cert";
            MIMETypeTbl["crt"] = "application/x-x509-ca-cert";
            MIMETypeTbl["xhtml"] = "application/xhtml+xml";
            MIMETypeTbl["xspf"] = "application/xspf+xml";
            // application/octet-stream
            MIMETypeTbl["bin"] = "application/octet-stream";
            MIMETypeTbl["exe"] = "application/octet-stream";
            MIMETypeTbl["dll"] = "application/octet-stream";
            MIMETypeTbl["deb"] = "application/octet-stream";
            MIMETypeTbl["dmg"] = "application/octet-stream";
            MIMETypeTbl["iso"] = "application/octet-stream";
            MIMETypeTbl["img"] = "application/octet-stream";
            MIMETypeTbl["msi"] = "application/octet-stream";
            MIMETypeTbl["msp"] = "application/octet-stream";
            MIMETypeTbl["msm"] = "application/octet-stream";
            // audio
            MIMETypeTbl["mid"] = "audio/midi";
            MIMETypeTbl["midi"] = "audio/midi";
            MIMETypeTbl["kar"] = "audio/midi";
            MIMETypeTbl["mp3"] = "audio/mpeg";
            MIMETypeTbl["ogg"] = "audio/ogg";
            MIMETypeTbl["m4a"] = "audio/x-m4a";
            MIMETypeTbl["ra"] = "audio/x-realaudio";
            // video
            MIMETypeTbl["3gpp"] = "video/3gpp";
            MIMETypeTbl["3gp"] = "video/3gpp";
            MIMETypeTbl["ts"] = "video/mp2t";
            MIMETypeTbl["mp4"] = "video/mp4";
            MIMETypeTbl["mpeg"] = "video/mpeg";
            MIMETypeTbl["mpg"] = "video/mpeg";
            MIMETypeTbl["mov"] = "video/quicktime";
            MIMETypeTbl["webm"] = "video/webm";
            MIMETypeTbl["flv"] = "video/x-flv";
            MIMETypeTbl["m4v"] = "video/x-m4v";
            MIMETypeTbl["mng"] = "video/x-mng";
            MIMETypeTbl["asx"] = "video/x-ms-asf";
            MIMETypeTbl["asf"] = "video/x-ms-asf";
            MIMETypeTbl["wmv"] = "video/x-ms-wmv";
            MIMETypeTbl["avi"] = "video/x-msvideo";
        }

        virtual ~MIMETypes();
    protected:
        std::map<std::string , std::string> MIMETypeTbl;
};

#endif