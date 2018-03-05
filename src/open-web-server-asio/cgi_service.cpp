#include "cgi_service.h"
#include <QStringList>
#include <QProcessEnvironment>
#include <iostream>

QProcess CgiService::qproccess_;

CgiService::CgiService()
{

}

bool CgiService::execute_(ClientRequest &client_request)
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    env.insert("REQUEST_METHOD", client_request.method == http_method::GET ? "GET" : "POST" ); //OK
    env.insert("REQUEST_SCHEME", "http"); //OK
    env.insert("DOCUMENT_ROOT", client_request.response.server_config_map_it->second.DocumentRoot); //"OK /Users/yiannis/Downloads/wordpress"
    env.insert("DOCUMENT_URI", QString::fromStdString(client_request.uri)); //OK
    env.insert("HTTP_HOST", QString::fromStdString(client_request.hostname));//OK     //localhost:12343");
    //env.insert("PATH", "/");
    env.insert("REQUEST_URI", QString::fromStdString(client_request.uri));   //"OK /wp-admin/setup-config.php");
    env.insert("QUERY_STRING", QString::fromStdString(client_request.query_string));
    env.insert("SCRIPT_FILENAME", client_request.response.absolute_hostname_and_requested_path);   //OK "/Users/yiannis/Downloads/wordpress/wp-admin/setup-config.php");
    env.insert("SCRIPT_NAME", QString::fromStdString(client_request.uri));//OK
    env.insert("REDIRECT_STATUS", "1");

    env.insert("SERVER_PROTOCOL", "HTTP/1.1"); //OK
    env.insert("SERVER_NAME", "localhost"); //OK
    env.insert("SERVER_PORT", "12343"); //OK

    env.insert("GATEWAY_INTERFACE", "CGI/1.1");
    env.insert("REDIRECT_STATUS", "200");
    //HTTP_COOKIE > wordpres....


    if (client_request.request_body.size() > 0){
        env.insert("CONTENT_TYPE", "application/x-www-form-urlencoded");
        env.insert("CONTENT_LENGTH", QString::number(client_request.request_body.size()));
    }
    qproccess_.setProcessEnvironment(env);

    //std::cout <<  client_request.response.absolute_hostname_and_requested_path.toStdString() << std::endl;
    qproccess_.start("/usr/local/bin/php-cgi"); //, QStringList() << client_request.response.absolute_hostname_and_requested_path);
    if (client_request.request_body.size()  > 0){
        qproccess_.write(client_request.request_body.data(), client_request.request_body.size());
        qproccess_.closeWriteChannel();
    }
    qproccess_.waitForFinished(); // sets current thread to sleep and waits for pingProcess end

   std::string output = qproccess_.readAllStandardOutput().toStdString();
   std::cout << output << std::endl;
   //std::vector<char> resp_vect(output.begin(), output.end());std::cout << output << "\r\n";

client_request.response.data_in.clear();
   client_request.response.data_in = std::vector<char>(output.begin(), output.end());
   client_request.response.parse(client_request.response.data_in, client_request.response.data_in.size(), client_request.response);

    /*
    QString envv = env.value("CONTENT_LENGTH");
    std::cout << "All variables" << std::endl;
    QString env_variable;
    QStringList paths_list = env.toStringList();

    foreach( env_variable, paths_list )
        std::cout << env_variable.toStdString() << std::endl;
    */

    return true;
    //return "";
}


