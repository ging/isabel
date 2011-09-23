/*
 * ISABEL: A group collaboration tool for the Internet
 * Copyright (C) 2009 Agora System S.A.
 * 
 * This fileName is part of Isabel.
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

namespace DVRB
{
    using System;
    using System.Collections.Generic;
    using System.Text;

    /**
     * This class is used to manage result values.
     */
    public class DVRBResult
    {
        public static readonly int OkCode = 0;
        public static readonly string OkDesc = "OK: The operation was successfully completed.";

        public static readonly int ERROR = -1;
        public static readonly string ERROR_DESC = "ERROR: Undefined error. Operation failed.";

        public static readonly int ERROR_IMP = -2;
        public static readonly string ERROR_IMP_DESC = "ERROR: Operation not implemented.";

        public static readonly int ERROR_FATAL = -3;
        public static readonly string ERROR_FATAL_DESC = "ERROR: fatal error";

        public static readonly int ERROR_ARG = -4;
        public static readonly string ERROR_ARG_DESC = "ERROR: Argument error";

        public static readonly int ERROR_THSTATE = -5;
        public static readonly string ERROR_THSTATE_DESC = "ERROR: Thread state error";

        public static readonly int ERROR_EXCEPTION = -6;
        public static readonly string ERROR_EXCEPTION_DESC = "ERROR: Exception raised";

        public static readonly int ERROR_PROFILE = -7;
        public static readonly string ERROR_PROFILE_DESC = "ERROR: Profile not found";

        public static readonly int ERROR_TIMEOUT = -8;
        public static readonly string ERROR_TIMEOUT_DESC = "ERROR: Method timeout";
       
        /**
         * Codigo de resultado.
         */
        private int code;

        /**
         * Descripcion del codigo de resultado.
         */
        private string description;

        /**
         * Constructor.
         */
        public DVRBResult()
        {
            this.code = OkCode;
            this.description = OkDesc;
        }

        /**
         * Constructor.
         * @param        code  The result code.
         * @param        description Text format result code string (OK, Login error, ...).
         */
        public DVRBResult(int code, string description)
        {
            this.code = code;
            this.description = description;
        }

        public int Code
        {
            get { return this.code; }
        }

        public string Description
        {
            get { return this.description; }
        }

        /**
         * Returns true if the operation was completed successfully, otherwise returns false.
         * @return       boolean 
         */
        public bool Succeeded
        {
            get { return this.code >= 0; }
        }
    }
}

