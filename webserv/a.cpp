#include <iostream>
#include <fstream>
#include <dirent.h>
#include <algorithm>
#include <vector>
#include <cstring>

std::vector<std::string> getDirectoryListing(const std::string& directory) {
    std::vector<std::string> listing;
    DIR* dir = opendir(directory.c_str());
    if (dir != NULL) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_name[0] != '.') {  // 숨김 파일 제외
                listing.push_back(entry->d_name);
            }
        }
        closedir(dir);
    }
    return listing;
}

void sortDirectoryListing(std::vector<std::string>& listing) {
    std::sort(listing.begin(), listing.end());
}

std::string generateDirectoryListingHTML(const std::string& directory, const std::vector<std::string>& listing) {
    std::string html;
    html += "<html><head><title>Directory Listing</title></head><body>\n";
    html += "<h1>Directory Listing: " + directory + "</h1>\n";
    html += "<ul>\n";
    for (size_t i = 0; i < listing.size(); ++i) {
        html += "<li><a href=\"";
        html += listing[i];
        html += "\">";
        html += listing[i];
        html += "</a></li>\n";
    }
    html += "</ul>\n";
    html += "</body></html>\n";
    return html;
}

std::string readFileContents(const std::string& file_path) {
    std::ifstream file(file_path.c_str(), std::ios::in | std::ios::binary);
    if (file) {
        std::string contents;
        file.seekg(0, std::ios::end);
        contents.resize(file.tellg());
        file.seekg(0, std::ios::beg);
        file.read(&contents[0], contents.size());
        file.close();
        return contents;
    }
    return "";
}

std::string generateFileHTML(const std::string& file_path) {
    std::string contents = readFileContents(file_path);
    std::string html;
    
    html += "<html><head><title>File: " + file_path + "</title></head><body>\n";
    html += "<h1>File: " + file_path + "</h1>\n";
    html += "<pre>\n";
    html += contents;
    html += "</pre>\n";
    html += "</body></html>\n";
    return html;
}

void handleRequest(const std::string& request_path) {
    std::string directory = ".";
    if (request_path != "/") {
        directory = request_path.substr(1);  // Remove leading slash
    }

    std::vector<std::string> listing = getDirectoryListing(directory);
    sortDirectoryListing(listing);
	std::vector<std::string> parentDirectory;
	parentDirectory.push_back("..");

    if (!listing.empty()) {
        std::string response;
        if (directory != ".") {
            response += generateDirectoryListingHTML("..", parentDirectory);  // Add parent directory link
        }
        response += generateDirectoryListingHTML(directory, listing);

        std::cout << "HTTP/1.1 200 OK\r\n";
        std::cout << "Content-Type: text/html\r\n";
        std::cout << "Content-Length: " << response.length() << "\r\n";
        std::cout << "\r\n";
        std::cout << response;
    } else {
        std::string file_path = directory;
        std::string file_contents = readFileContents(file_path);
        if (!file_contents.empty()) {
            std::string response = generateFileHTML(file_path);

            std::cout << "HTTP/1.1 200 OK\r\n";
            std::cout << "Content-Type: text/html\r\n";
            std::cout << "Content-Length: " << response.length() << "\r\n";
            std::cout << "\r\n";
            std::cout << response;
        } else {
            std::cout << "HTTP/1.1 404 Not Found\r\n";
            std::cout << "Content-Type: text/plain\r\n";
            std::cout << "Content-Length: 9\r\n";
            std::cout << "\r\n";
            std::cout << "Not Found";
        }
    }
}

int main() {
    std::string request_path = "/docs";

    handleRequest(request_path);

    return 0;
}