#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include "vncauth.h"
#include "d3des.h"

#ifdef __BUILD_FOR_WINXP
#define random() rand()
#define srandom(x) srand(x)
#endif

using namespace libvnc;

/*
* We use a fixed key to store passwords, since we assume that our local
* file system is secure but nonetheless don't want to store passwords
* as plaintext.
*/
unsigned char fixedkey[8] = {23,82,107,6,35,78,88,7};


/*
* Encrypt a password and store it in a file.  Returns 0 if successful,
* 1 if the file could not be written.
*/

int
CVncAuth::vncEncryptAndStorePasswd(char *passwd, char *fname)
{
    FILE *fp;
    unsigned int i;
    unsigned char encryptedPasswd[8];

    if ((fp = fopen(fname,"w")) == NULL) return 1;
#ifdef __BUILD_FOR_LINUX
    chmod(fname, S_IRUSR|S_IWUSR);
#endif
    /* pad password with nulls */

    for (i = 0; i < 8; i++) {
        if (i < strlen(passwd)) {
            encryptedPasswd[i] = passwd[i];
        } else {
            encryptedPasswd[i] = 0;
        }
    }

    /* Do encryption in-place - this way we overwrite our copy of the plaintext
    password */

    CD3Des::deskey(fixedkey, CD3Des::EN0);
    CD3Des::des(encryptedPasswd, encryptedPasswd);

    for (i = 0; i < 8; i++) {
        putc(encryptedPasswd[i], fp);
    }

    fclose(fp);
    return 0;
}


/*
* Decrypt a password from a file.  Returns a pointer to a newly allocated
* string containing the password or a null pointer if the password could
* not be retrieved for some reason.
*/
char *
CVncAuth::vncDecryptPasswdFromFile(char *fname)
{
    FILE *fp;
    int i, ch;
    unsigned char *passwd = new unsigned char[9];

    if ((fp = fopen(fname,"r")) == NULL) return NULL;

    for (i = 0; i < 8; i++) {
        ch = getc(fp);
        if (ch == EOF) {
            fclose(fp);
            return NULL;
        }
        passwd[i] = ch;
    }

    fclose(fp);

    CD3Des::deskey(fixedkey, CD3Des::DE1);
    CD3Des::des(passwd, passwd);

    passwd[8] = 0;

    return (char *)passwd;
}


/*
* Generate CHALLENGESIZE random bytes for use in challenge-response
* authentication.
*/
void
CVncAuth::vncRandomBytes(unsigned char *bytes)
{
    int i;
    unsigned int seed = (unsigned int) time(0);

    srandom(seed);
    for (i = 0; i < CHALLENGESIZE; i++) {
        bytes[i] = (unsigned char)(random() & 255);    
    }
}


/*
* Encrypt CHALLENGESIZE bytes in memory using a password.
*/
void
CVncAuth::vncEncryptBytes(unsigned char *bytes,const char *passwd)
{
    unsigned char key[8];
    unsigned int i;

    /* key is simply password padded with nulls */

    for (i = 0; i < 8; i++) {
        if (i < strlen(passwd)) {
            key[i] = passwd[i];
        } else {
            key[i] = 0;
        }
    }

    CD3Des::deskey(key, CD3Des::EN0);

    for (i = 0; i < CHALLENGESIZE; i += 8) {
        CD3Des::des(bytes+i, bytes+i);
    }
}
