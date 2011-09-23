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
    using System.Net;
    using System.Text;
    using NLog;

    internal class FlashPolicyServer
    {
        /**
         * Max receiver buffer.
         */
        private const int MaxPktSize = 1500;

        /**
         * Data send/receive timeout.
         */
        private const int WaitTimeout = 5000;

        /**
         * The server port.
         */
        private const int PolicyPort = 843;

        /**
         * The "magic" string to get policy fileName
         */
        private const string PolicyRequest = "<policy-fileName-request/>";

        /**
         * The policy fileName filename.
         */
        private const string PolicyFile = "FlashPolicyFile.xml";

        /**
         * Clase Nlog.
         */
        private static Logger log = LogManager.GetCurrentClassLogger();

        /**
         * Boolean to know if server is accepting connections.
         */
        private bool listening = false;

        /**
         * Tcp server.
         */
        private System.Net.Sockets.TcpListener server;

        /**
         * Constructor.
         */
        public FlashPolicyServer()
        {
            this.server = new System.Net.Sockets.TcpListener(IPAddress.Any, PolicyPort);                        
        }

        /**
         * Delegate used for AcceptProcessor
        */
        private delegate void AcceptProcessorDelegate(FlashPolicyServer server);

        /**
         * Start the server.
         */
        public void Start()
        {
            this.server.Start();
            log.Trace("Server started at port " + PolicyPort.ToString());
            this.listening = true;
            AcceptProcessorDelegate processor = new AcceptProcessorDelegate(this.AcceptProcessor);
            processor.BeginInvoke(this, null, null);             
        }

        /**
         * Stop the server.
         */
        public void Stop()
        {
            this.server.Stop();
            this.listening = false;
        }

        /**
         * Wait clients in other thread.
         */
        private void AcceptProcessor(FlashPolicyServer server)
        {
            log.Debug("BeginAcceptTcpClient");
            IAsyncResult asyncResult = this.server.BeginAcceptTcpClient(this.DoAcceptTcpClientCallback, this);
        }

        /**
         * New client callback.
         */        
        private void DoAcceptTcpClientCallback(IAsyncResult ar) 
        {
            FlashPolicyServer this_class = (FlashPolicyServer)ar.AsyncState;
            this_class.ProcessClient(ar);
        }

        /**
         * Process the client connection.
         */        
        private void ProcessClient(IAsyncResult ar)
        {
            System.Net.Sockets.Socket client = null;
            try
            {
                // Get socket
                client = this.server.EndAcceptSocket(ar);
                client.ReceiveTimeout = WaitTimeout; // Set timeouts
                client.SendTimeout = WaitTimeout;
                log.Debug("New client connected.");

                // Receive data
                byte[] buffer = new byte[MaxPktSize];
                if (client.Receive(buffer, MaxPktSize, System.Net.Sockets.SocketFlags.None) > 0)
                {
                    // Get string from buffer
                    string data = Encoding.ASCII.GetString(buffer);
                    data = data.Substring(0, data.IndexOf('\0'));
                    log.Debug("<-- " + data);

                    if (data == PolicyRequest) // If data is policy request 
                    {                           // we send the policy fileName
                        log.Debug("Sending policy request...");
                        client.SendFile(PolicyFile);
                    }
                }

                // Start listenning
                this.server.BeginAcceptTcpClient(this.DoAcceptTcpClientCallback, this);
            }
            catch (System.Exception e)
            {
                log.Error("Error processing client : " + e.Message);
            }

            if (client != null)
            {
                client.Close(); // close socket
            }

            if (this.listening)
            {
                AcceptProcessorDelegate processor = new AcceptProcessorDelegate(this.AcceptProcessor);
                processor.BeginInvoke(this, null, null);
            }
        }
    }
}
