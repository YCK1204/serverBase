#include "../header/Mime.hpp"

Mime::Mime() {
    status[100] = "Continue";
    status[101] = "Switching Protocols";
    status[102] = "Processing";
    status[103] = "Early Hints";

    /* success */
    status[200] = "OK";
    status[201] = "Created";
    status[202] = "Accepted";
    status[203] = "Non-Authoritative Information";
    status[204] = "No Content";
    status[205] = "Reset Content";
    status[206] = "Partial Content";
    status[207] = "Multi-Status";
    status[208] = "Already Reported";
    status[226] = "IM Used";
    
    /* redirection */
    status[300] = "Multiple Choices";
    status[301] = "Moved Permanently";
    status[302] = "Found";                // MOVED_TEMPORARILY 
    status[303] = "See Other";
    status[304] = "Not Modified";
    status[305] = "Use Proxy";
    status[307] = "Temporary Redirect";
    status[308] = "Permanent Redirect";

    /* client error */
    status[400] = "Bad Request";
    status[401] = "Unauthorized";
    status[402] = "Payment Required";
    status[403] = "Forbidden";
    status[404] = "Not Found";
    status[405] = "Method Not Allowed";
    status[406] = "Not Acceptable";
    status[407] = "Proxy Authentication Required";
    status[408] = "Request Timeout";
    status[409] = "Conflict";
    status[410] = "Gone";
    status[411] = "Length Required";
    status[412] = "Precondition Failed";
    status[413] = "Request Entity Too Large";
    status[414] = "Request-URI Too Long";
    status[415] = "Unsupported Media Type";
    status[416] = "Requested Range Not Satisfiable";
    status[417] = "Expectation Failed";
    status[426] = "Upgrade Required";
    status[431] = "Request Header Fields Too Large";

    /* server errors */
    status[500] = "Internal Server Error";
    status[501] = "Not Implemented";
    status[502] = "Bad Gateway";
    status[503] = "Service Unavailable";
    status[504] = "Gateway Timeout";
    status[505] = "HTTP Version Not Supported";

    type["html"]     = "text/html";
    type["htm"]      = "text/html";
    type["shtml"]    = "text/html";
    type["css"]      = "text/css";
    type["xml"]      = "text/xml";
    type["txt"]      = "text/plain";
    type["jad"]      = "text/vnd.sun.j2me.app-descriptor";
    type["wml"]      = "text/vnd.wap.wml";
    type["htc"]      = "text/x-component";
    type["mml"]      = "text/mathml";
    // image
    type["gif"] = "image/gif";
    type["jpeg"] = "image/jpeg";
    type["jpg"] = "image/jpeg";
    type["png"] = "image/png";
    type["svg"] = "image/svg+xml";
    type["svgz"] = "image/svg+xml";
    type["tiff"] = "image/tiff";
    type["tif"] = "image/tiff";
    type["wbmp"] = "image/vnd.wap.wbmp";
    type["webp"] = "image/webp";
    type["ico"] = "image/x-icon";
    type["jng"] = "image/x-jng";
    type["bmp"] = "image/x-ms-bmp";
    // font
    type["woff"] = "font/woff";
    type["woff2"] = "font/woff2";
    // application
    type["js"] = "application/javascript";
    type["atom"] = "application/atom+xml";
    type["rss"] = "application/rss+xml";
    type["json"] = "application/json";
    type["jar"] = "application/java-archive";
    type["war"] = "application/java-archive";
    type["ear"] = "application/java-archive";
    type["pdf"] = "application/pdf";
    type["doc"] = "application/msword";
    type["xls"] = "application/vnd.ms-excel";
    type["ppt"] = "application/vnd.ms-powerpoint";
    type["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    type["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    type["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
    type["wasm"] = "application/wasm";
    type["zip"] = "application/zip";
    type["7z"] = "application/x-7z-compressed";
    type["cco"] = "application/x-cocoa";
    type["jardiff"] = "application/x-java-archive-diff";
    type["jnlp"] = "application/x-java-jnlp-file";
    type["run"] = "application/x-makeself";
    type["pl"] = "application/x-perl";
    type["pm"] = "application/x-perl";
    type["prc"] = "application/x-pilot";
    type["pdb"] = "application/x-pilot";
    type["rar"] = "application/x-rar-compressed";
    type["rpm"] = "application/x-redhat-package-manager";
    type["sea"] = "application/x-sea";
    type["swf"] = "application/x-shockwave-flash";
    type["sit"] = "application/x-stuffit";
    type["tcl"] = "application/x-tcl";
    type["tk"] = "application/x-tcl";
    type["der"] = "application/x-x509-ca-cert";
    type["pem"] = "application/x-x509-ca-cert";
    type["crt"] = "application/x-x509-ca-cert";
    type["xpi"] = "application/x-xpinstall";
    type["der"] = "application/x-x509-ca-cert";
    type["pem"] = "application/x-x509-ca-cert";
    type["crt"] = "application/x-x509-ca-cert";
    type["xhtml"] = "application/xhtml+xml";
    type["xspf"] = "application/xspf+xml";
    // application/octet-stream
    type["bin"] = "application/octet-stream";
    type["exe"] = "application/octet-stream";
    type["dll"] = "application/octet-stream";
    type["deb"] = "application/octet-stream";
    type["dmg"] = "application/octet-stream";
    type["iso"] = "application/octet-stream";
    type["img"] = "application/octet-stream";
    type["msi"] = "application/octet-stream";
    type["msp"] = "application/octet-stream";
    type["msm"] = "application/octet-stream";
    // audio
    type["mid"] = "audio/midi";
    type["midi"] = "audio/midi";
    type["kar"] = "audio/midi";
    type["mp3"] = "audio/mpeg";
    type["ogg"] = "audio/ogg";
    type["m4a"] = "audio/x-m4a";
    type["ra"] = "audio/x-realaudio";
    // video
    type["3gpp"] = "video/3gpp";
    type["3gp"] = "video/3gpp";
    type["ts"] = "video/mp2t";
    type["mp4"] = "video/mp4";
    type["mpeg"] = "video/mpeg";
    type["mpg"] = "video/mpeg";
    type["mov"] = "video/quicktime";
    type["webm"] = "video/webm";
    type["flv"] = "video/x-flv";
    type["m4v"] = "video/x-m4v";
    type["mng"] = "video/x-mng";
    type["asx"] = "video/x-ms-asf";
    type["asf"] = "video/x-ms-asf";
    type["wmv"] = "video/x-ms-wmv";
    type["avi"] = "video/x-msvideo";
}
Mime::~Mime() {}

std::string Mime::getStatus(const int &n) {
    std::map<int, std::string>::iterator it = this->status.find(n);
    if (it == this->status.end())
        it = this->status.find(404);
    return (it->second);
}

std::string Mime::getType(const std::string &type) {
    std::map<std::string, std::string>::iterator it = this->type.find(type);
    if (it == this->type.end())
        return "application/octet-stream";
    return (this->type.find(type)->second);
}