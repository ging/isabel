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
    using System.ServiceProcess;
    using NLog;

    public class Program 
    {
        public static void Main(string[] args)
        {
            // checks if we are running as a service or as a console interactive application
            // this allows us to run the same code interactively (for debug prupourses) or
            // in production
            if (!Environment.UserInteractive)
            {
                ServiceBase[] servicesToRun;
                servicesToRun = new ServiceBase[] { new DVRBService() };
                ServiceBase.Run(servicesToRun);
            }
            else
            {
                DVRBService service = new DVRBService();
                service.DVRBStart(null);
                System.Console.WriteLine("Press Enter to stop the service");
                System.Console.ReadLine();
                service.DVRBStop();
            }
        }
    }
}
