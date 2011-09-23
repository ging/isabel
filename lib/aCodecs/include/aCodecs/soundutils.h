/*
 * ISABEL: A group collaboration tool for the Internet
 * Copyright (C) 2009 Agora System S.A.
 * 
 * This file is part of Isabel.
 * 
 * Isabel is free software: you can redistribute it and/or modify
 * it under the terms of the Affero GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Isabel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Affero GNU General Public License for more details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __SOUND_UTILS_H__
#define __SOUND_UTILS_H__

class SoundUtils
{
public:

    /**
    * Devuelve la potencia de un buffer de audio.
    * El valor esta entre -50 y 0;
    * @param buffer Buffer con los datos.
    * @param len Longitud en bytes del buffer.
    */
    static double evalPower(unsigned char* buffer, int len);

    /**
    * Multiplica un buffer por un factor.
    * @param buffer Buffer con los datos.
    * @param len Longitud en bytes del buffer.
    * @param dbs Factor a aplicar.
    */
    static void bufferAtenuator(unsigned char *buffer, int len, float dbs);

    class Resampler
    {
    private:
        short last;
    public:
        Resampler(void);

        int resample(const unsigned char *indata,
                     int indatas,
                     int inrate,
                     unsigned char *outdata,
                     int outdatas,
                     int outrate,
                     bool useLast = true
                    );

    };

    class EchoSuppressor
    {
    public:
        /**
        * Número de periodos sobre los que se hace
        * media para calcular la atenuación.
        */
        static const int MAX_ECHO_WINDOW = 100;

        static int level;

        static int echo_window;

        static int echo_threshold;

    private:

        /**
        * Umbral a partir del cual se empieza a aplicar el cancelador de eco.
        */
        static const int DEFAULT_ECHO_THRESHOLD = -35;

        static double data[MAX_ECHO_WINDOW];

    public:

        /**
        * Devuelve el valor en que se debe atenuar la entrada.
        */
        static float getAttenuation(void);

        /**
        * Pasa un valor de potencia para calcular la atenuación.
        */
        static void nextData(double pow);
    };

    class CAG
    {
    public:
        /**
        * Número de periodos sobre los que se hace media
        * para calcular la atenuación.
        */
        static const int MAX_CAG_WINDOW = 50;

        static int level;

        static int cag_window;

        static int cag_threshold;

    private:

        /**
        * Umbral a partir del cual se empieza a aplicar el CAG
        */
        static const int DEFAULT_CAG_THRESHOLD = -30;

        static double data[MAX_CAG_WINDOW];

    public:

        /**
        * Devuelve el valor en que se debe atenuar la entrada.
        */
        static float getAttenuation(void);

        /**
        * Pasa un valor de potencia para calcular la atenuación.
        */
        static void nextData(double pow);
    };
};

#endif

