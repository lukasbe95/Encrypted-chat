#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#define FAIL    -1

int Open_Connection(const char *hostname, int port){
    int sd;
    struct hostent *host;
    struct sockaddr_in addr;
    if ( (host = gethostbyname(hostname)) == NULL ){
        perror(hostname);
        abort();
    }
    sd = socket(PF_INET, SOCK_STREAM, 0);
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = *(long*)(host->h_addr);
    if ( connect(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0 ){
        close(sd);
        perror(hostname);
        abort();
    }
    return sd;
}

SSL_CTX* Init_CTX(void){
    SSL_METHOD *method;
    SSL_CTX *ctx;
    OpenSSL_add_all_algorithms();  /* Load cryptos, et.al. */
    SSL_load_error_strings();   /* Bring in and register error messages */
    method = TLSv1_2_client_method();  /* Create new client-method instance */
    ctx = SSL_CTX_new(method);   /* Create new context */
    if ( ctx == NULL ){
        ERR_print_errors_fp(stderr);
        abort();
    }
    return ctx;
}

void Show_Certs(SSL* ssl){
    X509 *cert;
    char *line;
    cert = SSL_get_peer_certificate(ssl);
    if ( cert != NULL ){
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line);
        X509_free(cert);
    } else {
        printf("Info: No client certificates configured.\n");
    }
}
void Connect_Client(SSL *ssl)
{
    char buf[1024] = {0};
    int sd, bytes, bytes_w;
    if ( SSL_accept(ssl) == FAIL ){
        ERR_print_errors_fp(stderr);
    } else {
        Show_Certs(ssl);
        scanf("%s", buf);
        bytes_w = SSL_write(ssl, buf, sizeof(buf));
        if (bytes_w > 0){
            buf[0] = '\0';
        }
        while(1){
            bytes = SSL_read(ssl, buf, sizeof(buf));
            if (bytes > 0){
                printf("<< %s\n", buf);
                buf[0] = '\0';
            }
            scanf("%s", buf);
            bytes_w = SSL_write(ssl, buf, sizeof(buf));
            if (bytes_w > 0){
                buf[0] = '\0';
            }
        }
    }
    sd = SSL_get_fd(ssl);
    SSL_free(ssl);
    close(sd);
}

int main(int count, char *strings[]){
    SSL_CTX *ctx;
    int server;
    SSL *ssl;
    char *hostname, *portnum;
    if ( count != 3 ){
        printf("usage: %s <hostname> <portnum>\n", strings[0]);
        exit(0);
    }
    SSL_library_init();
    hostname=strings[1];
    portnum=strings[2];
    ctx = Init_CTX();
    server = Open_Connection(hostname, atoi(portnum));
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, server);
    if ( SSL_connect(ssl) == FAIL ){
        ERR_print_errors_fp(stderr);
    }else{
        printf("\nConnected with %s encryption\n", SSL_get_cipher(ssl));
        Show_Certs(ssl);
        Connect_Client(ssl);
        SSL_free(ssl);
    }
    close(server);
    SSL_CTX_free(ctx);
    return 0;
}
