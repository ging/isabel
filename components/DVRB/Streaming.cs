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
    using Microsoft.WindowsMediaServices.Interop;
    using NLog;

    /**
     * Clase encargada de la gestion del servidor de streaming.
     */
    internal class Streaming
    {
        /**
         * Clase Nlog.
         */
        private static Logger log = LogManager.GetCurrentClassLogger();

        /**
         * Live state.
         */
        private State liveState;

        /**
         * OnDemand state.
         */
        private State onDemandState;
      
        /**
         * Servidor de streaming.
         */
        private WMSServer wmServer;

        /**
         * Boolean to enable/disable streaming.
         */
        private Modes mode;

        /**
         * Broadcast publish point.
         */
        private IWMSBroadcastPublishingPoint livePublishingPoint;

        /**
         * OnDemand publish point.
         */
        private IWMSOnDemandPublishingPoint onDemandPublishingPoint;

        /**
         * Constructor.
         */
        public Streaming()
        {
            log.Trace("Creating streaming server");
            this.livePublishingPoint = null;
            this.onDemandPublishingPoint = null;
            this.onDemandState = State.Stopped;
            this.liveState = State.Stopped;
            this.mode = Modes.None;

            this.wmServer = new WMSServerClass();

            // Remove existing publishing points
            for (int i = 0; i < this.wmServer.PublishingPoints.length; ++i)
            {
                log.Trace("Existing publishing points = " + this.wmServer.PublishingPoints.length);
                if (this.wmServer.PublishingPoints[i].Type == WMS_PUBLISHING_POINT_TYPE.WMS_PUBLISHING_POINT_TYPE_BROADCAST || 
                    this.wmServer.PublishingPoints[i].Type == WMS_PUBLISHING_POINT_TYPE.WMS_PUBLISHING_POINT_TYPE_ON_DEMAND)
                {
                    try
                    {                        
                        this.wmServer.PublishingPoints.Remove(i);
                        i--;
                    }
                    catch (Exception e)
                    {
                        log.ErrorException("Exception raised while removing publishing point: " + e.Message, e);
                    }
                }
            }
        }

        /**
         * Destructor.
         */
        ~Streaming()
        {
            log.Trace("Destroy class");
        }

        /**
         * Streaming caps.
         */
        public enum Modes : ulong
        {
            None = 0x00,
            OnDemand = 0x01,
            Live = 0x02,
            All = 0x03
        }

        /**
         * Streaming/vod states.
         */
        public enum State : ulong
        {
            Stopped = 0x00,
            Running = 0x01,
        }

        /**
         * Enable/Disable streaming capabilities.
         */
        public Modes Active
        {
            get
            {
                return this.mode;
            }

            set
            {
                this.mode = value;
                log.Trace("Server Live: " + ((this.mode & Modes.Live) != 0));
                log.Trace("Server OnDemand: " + ((this.mode & Modes.OnDemand) != 0));
            }
        }

        /**
         * Get Live state.
         */
        public State LiveStreamingState
        {
            get { return this.liveState; }
        }

        /**
         * Get OnDemand state.
         */
        public State OnDemandState
        {
            get { return this.onDemandState; }
        }

        /**
         * Set WMEncoder pull address and port.
         * @param sourceHostName : Address where Windows Media Encoder is running (ussually 127.0.0.1).
         * @param sourcePort : Port where windows Media Encoder is pulling.
         * @param allow_unicast : Boolean to enable/disable unicast clients.
         * @param output_addr : Multicast output address.
         * @param output_port : Multicast output port.
         */
        public DVRBResult SetLiveStreamingInfo(
                                    string sourceHostName, 
                                    int sourcePort, 
                                    bool allow_unicast,
                                    string output_addr, 
                                    int output_port)
        {
            log.Trace("Set LiveStreamingInfo: " + sourceHostName + ":" + sourcePort + "-->" + output_addr + ":" + output_port);                
                
            string sourceUrl = "http://" + sourceHostName + ":" + sourcePort;
            if (this.livePublishingPoint == null)
            {
                // Add a new broadcast publishing point.
                this.livePublishingPoint = (IWMSBroadcastPublishingPoint)this.wmServer.PublishingPoints.Add(
                                                       "Live",
                                                       WMS_PUBLISHING_POINT_CATEGORY.WMS_PUBLISHING_POINT_BROADCAST,
                                                       sourceUrl);
                this.livePublishingPoint.AnnouncementStreamFormats.Add(sourceUrl);
            }
            else
            {
                this.livePublishingPoint.Stop();
                this.liveState = State.Stopped;
                this.livePublishingPoint.Path = sourceUrl;
            }

            // Configure unicast session
            this.livePublishingPoint.AllowClientsToConnect = allow_unicast;

            // Configure multicast session
            for (int i = 0; i < this.livePublishingPoint.BroadcastDataSinks.length; i++)
            {
                IWMSPlugin plugin = this.livePublishingPoint.BroadcastDataSinks[i];
                try
                {
                    IWMSAdminMulticastSink multicast = (IWMSAdminMulticastSink)plugin.CustomInterface;
                    multicast.DestinationMulticastIPAddress = output_addr;
                    multicast.DestinationMulticastPort = output_port;
                    plugin.Enabled = true;
                }
                catch (System.Exception) 
                { 
                }
            }

            this.livePublishingPoint.Announce();            
            return new DVRBResult();
        }

        /**
         * Set video on demand info.
         * @param path : Directory to share.         
         */
        public DVRBResult SetVoDInfo(string path)
        {
            bool allow_clients = false;
            if (this.onDemandPublishingPoint == null)
            {
                log.Trace("Set VoDInfo: addr = " + path);
                this.onDemandPublishingPoint = (IWMSOnDemandPublishingPoint)this.wmServer.PublishingPoints.Add(
                                                     "OnDemand",
                                                     WMS_PUBLISHING_POINT_CATEGORY.WMS_PUBLISHING_POINT_ON_DEMAND,
                                                     path);
                this.onDemandPublishingPoint.AllowClientsToConnect = false;
            }
            else
            {
                this.onDemandPublishingPoint.AllowClientsToConnect = false;
                log.Trace("Change VoDInfo: addr = " + path);
                allow_clients = this.onDemandPublishingPoint.AllowClientsToConnect;
                this.onDemandPublishingPoint.Path = path;
            }

            this.onDemandPublishingPoint.EnableClientWildcardDirectoryAccess = false;
            this.onDemandState = State.Stopped;
            return new DVRBResult();
        }

        /**
         * Start streaming.
         */
        public DVRBResult Start()
        {
            DVRBResult result = new DVRBResult();
            if ((this.mode & Modes.Live) != 0)
            {
                if (this.livePublishingPoint != null)
                {
                    this.livePublishingPoint.Start();
                    this.liveState = State.Running;
                }
                else
                {
                    log.Error("Start:: Live is Active but not configured.");
                    result = new DVRBResult(DVRBResult.ERROR, "Start:: Live is Active but not configured.");
                }
            }

            if ((this.mode & Modes.OnDemand) != 0)
            {
                if (this.onDemandPublishingPoint != null)
                {
                    this.onDemandPublishingPoint.AllowClientsToConnect = true;
                    this.onDemandPublishingPoint.EnableClientWildcardDirectoryAccess = true;
                    this.onDemandState = State.Running;
                }
                else
                {
                    log.Error("Start:: OnDemand is Active but not configured.");
                    result = new DVRBResult(DVRBResult.ERROR, "Start:: OnDemand is Active but not configured.");
                }
            }

            return result;
        }

        /**
         * Stop streaming.
         */
        public DVRBResult Stop()
        {
            if (this.livePublishingPoint != null)
            {
                this.livePublishingPoint.Stop();
            }

            if (this.onDemandPublishingPoint != null)
            {
                this.onDemandPublishingPoint.AllowClientsToConnect = false;
                this.onDemandPublishingPoint.EnableClientWildcardDirectoryAccess = false;
            }

            this.liveState = State.Stopped;
            this.onDemandState = State.Stopped;
            return new DVRBResult();
        }

        /**
         * Saves NSC announce to specified directory.
         * @param path : Full path + fileName name where the announce fileName is going to be saved (fileName should end with *.nsc extension).
         */
        public DVRBResult PublishNSCAnnounce(string path)
        {
            DVRBResult result = new DVRBResult();
            if (this.livePublishingPoint != null)
            {
                try
                {
                    this.livePublishingPoint.Announce();
                    this.livePublishingPoint.AnnounceToNSCFile(path, true);
                }
                catch (System.Exception e)
                {
                    log.ErrorException("Error creating announce in " + path + " : " + e.Message, e);
                    result = new DVRBResult(DVRBResult.ERROR_EXCEPTION, e.Message);
                }                
            }
            else
            {
                result = new DVRBResult(DVRBResult.ERROR, DVRBResult.ERROR_DESC);
            }

            return result;
        }
    }
}
