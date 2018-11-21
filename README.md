# WebProxyServerForHTTPS-ftp


Introduction

In this project you will extend the web server to perform HTTP fetching and FTP mediation. This means your server will receive HTTP requests (e.g. GET http://site.com/file.xyz) or FTP requests (e.g. GET ftp://ftp.site.com/path/to/file) and send back HTTP responses. When a HTTP request is received you will fetch the object (/file.xyz) from the named site (site.com) and return a normal HTTP response. However, when a FTP request is received, your proxy will ‘learn’ how to handle this request, perform the FTP transaction and return the requested file over HTTP - as a HTTP response.

Requirements

Your server should speak a subset of HTTP/1.1 protocol (RFC 2616). You should ensure that you are able to handle the following:

All the requirements and conditions in the previous project (MP1) apply in full force - in particular you cannot use fork() or multi processes and you should not use more than 30MB of memory and 20 file descriptors.
GET requests work and images and arbitrary length binary files are transferred correctly. (For FTP requests you should be able to handle files of type ISO, images, gz/Zip and plain text/HTML files.)
Your server should properly handle Full-Requests (RFC 1945 section 4.1) up to 65535 bytes. You may close the connection if a Full-Request is larger than that.
You must support URLs with a numerical IP address instead of the server name (e.g. http://128.181.0.31/).
Note - all requests should receive a HTTP response (even if they are bad, invalid or whatever) and your server should not crash under any circumstances.

In addition you should be able to handle some negative test cases:

Split request (more than one chunk request)
Large request or response (65KB request and up to 2GB - yes gigabytes - response)
Bad Connect (connect to a bad port e.g. www.njit.edu:33333)
Malformed request (you should think of malformed requests)
Huge request (requests greater than 65KB up to 1MB)
Drop connection (client or server drops the connection and goes away abruptly)


Run(C)(Me): ./a.out -Xmx3m Web 11115
Run(Java)(Team): java -Xmx3m Web 11115



