#ifndef __VNC_AUTH_H__
#define __VNC_AUTH_H__

namespace libvnc
{
    class CVncAuth
    {
    public:

        static const int CHALLENGESIZE = 16;    

    public:      

        /*
        * Encrypt CHALLENGESIZE bytes in memory using a password.
        */
        static void  vncEncryptBytes(unsigned char *bytes, const char *passwd);
        /*
        * Encrypt a password and store it in a file.  Returns 0 if successful,
        * 1 if the file could not be written.
        */
        static int   vncEncryptAndStorePasswd(char *passwd, char *fname);
        /*
        * Decrypt a password from a file.  Returns a pointer to a newly allocated
        * string containing the password or a null pointer if the password could
        * not be retrieved for some reason.
        */
        static char* vncDecryptPasswdFromFile(char *fname);
        /*
        * Generate CHALLENGESIZE random bytes for use in challenge-response
        * authentication.
        */
        static void  vncRandomBytes(unsigned char *bytes);
    };
}

#endif