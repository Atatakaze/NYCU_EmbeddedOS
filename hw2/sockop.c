#include "sockop.h"

int passivesock(const char *service, const char *transport, int qlen)
{
    struct servent *pse;
    struct sockaddr_in sin;
    int s, type;
    
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    
    if(pse = getservbyname(service, transport))
        sin.sin_port = htons(ntohs((unsigned short)pse->s_port));
    else if((sin.sin_port = htons((unsigned short)atoi(service)))==0)
        errexit("Can't find \"%s\" service entry\n",service);
        
    if(strcmp(transport,"udp")==0)
        type = SOCK_DGRAM;
    else
        type = SOCK_STREAM;
    
    s = socket(PF_INET, type, 0);
    if(bind(s, (struct sockaddr *)&sin,sizeof(sin))<0)
        errexit("Can't bind to port %s: %s\n",service,strerror(errno));
    
    if(type == SOCK_STREAM && listen(s,qlen)<0)
        errexit("Can't listen on port %s: %s\n",service, strerror(errno));
    return s;
}

int connectsock(const char *host, const char *service, const char *transport)
{
    struct hostent *phe;
    struct servent *pse;
    struct sockaddr_in sin;
    int s, type;
    
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    if(pse = getservbyname(service, transport))
        sin.sin_port = pse->s_port;
    else if((sin.sin_port = htons((unsigned short)atoi(service)))==0)
        errexit("Can't find \"%s\" service entry\n",service);
    
    if(strcmp(transport,"udp")==0)
        type = SOCK_DGRAM;
    else
        type = SOCK_STREAM;
    
    s = socket(PF_INET, type, 0);
    
    if(s<0)
        errexit("Can't create socket: %s\n",strerror(errno));
    
    if(connect(s,(struct sockaddr *)&sin, sizeof(sin))<0)
        errexit("Can't connect to %s.%s: %s\n", host,service,strerror(errno));
    
    return s;


}

