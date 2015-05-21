#ifndef BRETLNET_COMMON_H
#define BRETLNET_COMMON_H

#include <exception>

enum Protocol {TCP, UDP, CONN_UDP};

class NetException : public std::exception {
    public:
        NetException(const char *msg, bool sysMsg=false) throw();
        ~NetException() throw();
        const char *what() const throw();
    private:
        std::string userMessage;
};

typedef NetException ClientException;
typedef NetException ServerException ;

#endif
