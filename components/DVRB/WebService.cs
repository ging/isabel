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
    using System.IO;
    using System.Net;
    using System.Text;
    using NLog;

    internal class WebService
    {
        /**
         * Connection addr between Encoder and Streaming 
         */
        private static readonly string internalAddr = "127.0.0.1";

        /**
         * Connection port between Encoder and Streaming 
         */
        private static readonly int internalPort = 8081;

        /**
         * Clase Nlog.
         */
        private static Logger log = LogManager.GetCurrentClassLogger();

        /**
         * Current DVRB configuration.
         */
        private XmlSchemas.dvrb dvrbConfig;

        /**
         * References to encoder class.
         */
        private Encoder encoder;

        /**
         * Reference to streaming class.
         */
        private Streaming streaming;

        /**
         * prefixes where the server starts. 
         * Should start with http:// or https://
         */
        private IEnumerable<string> uriPrefixes;

        /**
         * HttpListener
         */
        private HttpListener listener;

        /**
         * Constructor for WebService
         * @param prefix string where the server should listen. Example "http://localhost:8080/"
         */
        public WebService(string prefix, Encoder encoder, Streaming streaming)
        {
            log.Debug("Creating Web Service");
            this.dvrbConfig = null;
            this.uriPrefixes = new string[] { prefix };
            this.encoder = encoder;
            this.streaming = streaming;

            // Create a listener.
            this.listener = new HttpListener();
        }

        // Delegate used for RequestProcessor
        private delegate void RequestProcessorDelegate(HttpListener listener);

        /**
         * Starts the HttpListener and the WebServices
         */
        public void StartWebServiceListener()
        {
            if (!HttpListener.IsSupported)
            {
                log.Error("Windows XP SP2 or Server 2003 is required to use the HttpListener class.");
                return;
            }

            // Add the prefixes.
            foreach (string s in this.uriPrefixes)
            {
                this.listener.Prefixes.Add(s);
            }

            this.listener.Start();
            log.Info("WebServer: Listening...");

            // A delegate should be created in order to prevent the thread being blocked
            RequestProcessorDelegate processor = new RequestProcessorDelegate(this.RequestProcessor);
            processor.BeginInvoke(this.listener, null, null);
        }

        /**
         * Method to Stop the Listener and the WebServices attached to it.
         */
        public void Stop()
        {
            try
            {
                this.listener.Stop();
            }
            catch (HttpListenerException)
            {
            }
        }

        private void RequestProcessor(HttpListener listener)
        {
            while (this.listener.IsListening)
            {
                // Calls GetContext method asynchronously
                IAsyncResult asyncResult = this.listener.BeginGetContext(new AsyncCallback(this.GetContextCallback), listener);
                log.Debug("Waiting For Request");

                // This ensures that the next call to GetContect asynchronously is done after
                // the current finishes (however, the processing of the context happens asynchronously in the callback thread)
                asyncResult.AsyncWaitHandle.WaitOne();
            }
        }

        /**
         * This callback is called when the listener receives a request or when it is stopped
         * @param ar the IAsyncResult of the BeginGetInvokeMethod
         */
        private void GetContextCallback(IAsyncResult ar)
        {
            HttpListener listener = (HttpListener)ar.AsyncState;
            log.Debug("GetContextCallback called");

            // If the listener has stopped we should handle a HttpListenerException
            try
            {
                HttpListenerContext context = listener.EndGetContext(ar);

                // Trace
                this.CreateResponse(context);
                log.Debug("Callback Finished");
            }
            catch (HttpListenerException)
            {
                log.Debug("Callback Finished due to Stop");
            }
        }

        /**
         * Parses and creates the consequent response
         * @param uri: the uri of the request
         * @param method: method of the request
         * @param content: content of the request
         * @param context: listener context
         */
        private void CreateResponse(HttpListenerContext context)
        {
            // Obtain a response and a request object
            HttpListenerRequest request = context.Request;

            // Get Uri and Content Data
            Uri uri = request.Url;
            System.IO.StreamReader reader = new System.IO.StreamReader(request.InputStream);
            string content = reader.ReadToEnd();

            // Get Method
            string method = request.HttpMethod;

            log.Debug("Processing: {0}", uri);

            // Splits the uri in a list of its parts
            IList<string> uriParts = this.SplitUri(uri);

            if (uriParts.Count > 1 && uriParts[1] == "dvrb")
            {
                // uri = /dvrb
                if (uriParts.Count == 2)
                {
                    switch (method)
                    {
                        case "GET":
                            this.GetDVRB(uri, method, content, context);
                            break;
                        case "PUT":
                            this.UpdateDVRB(uri, method, content, context);
                            break;
                        default:
                            this.NotAllowed(uri, method, content, context);
                            break;
                    }
                }

                // uri = /dvrb/state
                else if (uriParts.Count == 3 && uriParts[2] == "state")
                {
                    switch (method)
                    {
                        case "GET":
                            this.GetState(uri, method, content, context);
                            break;
                        case "PUT":
                            this.UpdateState(uri, method, content, context); // DONE
                            break;
                        default:
                            this.NotAllowed(uri, method, content, context);
                            break;
                    }
                }

                // uri = /dvrb/info
                else if (uriParts.Count == 3 && uriParts[2] == "info")
                {
                    if (method == "GET")
                    {
                        this.GetInfo(uri, method, content, context);
                    }
                    else
                    {
                        this.NotAllowed(uri, method, content, context);
                    }
                }
                else if (uriParts.Count == 4 && uriParts[2] == "system")
                {
                    // uri = /dvrb/system/info
                    if (uriParts[3] == "info")
                    {
                        if (method == "GET")
                        {
                            this.GetSystemInfo(uri, method, content, context); // DONE
                        }
                        else
                        {
                            this.NotAllowed(uri, method, content, context);
                        }
                    }

                    // uri = /dvrb/system/"any other non-existing resource"
                    else
                    {
                        this.NotFound(uri, method, content, context);
                    }
                }
            }

            // uri = "non-existing resource"
            else
            {
                this.NotFound(uri, method, content, context);
            }
        }

        private void UpdateState(Uri uri, string method, string content, HttpListenerContext context)
        {
            HttpListenerResponse response = context.Response;
            try
            {
                DVRBResult result;

                // parse xml body
                XmlSchemas.server_state body = new XmlSchemas.server_state();
                body.ReadXml(new System.IO.StringReader(content));
                log.Debug("updateState:: Recording --> " + body.recording[0].state);
                log.Debug("updateState:: Streaming --> " + body.streaming[0].state);
                log.Debug("updateState:: vod --> " + body.vod[0].state);

                if (this.dvrbConfig == null && !(body.recording[0].state == "stop" && body.streaming[0].state == "stop" && body.vod[0].state == "stop"))
                {
                    throw new System.Exception("System is not configured");
                }

                // recording management
                switch (body.recording[0].state)
                {
                    case "stop":
                        result = this.encoder.EnableRecording(false);
                        if (!result.Succeeded)
                        {
                            throw new System.Exception("encoder.EnableRecording returns ERROR [" + result.Code + "]:" + result.Description);
                        }

                        break;
                    case "play":
                        result = this.encoder.EnableRecording(true);
                        if (!result.Succeeded)
                        {
                            throw new System.Exception("encoder.EnableRecording returns ERROR [" + result.Code + "]:" + result.Description);
                        }

                        break;
                    default:
                        throw new System.Exception("Unknown record state received: " + body.recording[0].state);
                }

                // streaming and OnDemand management
                Streaming.Modes state = Streaming.Modes.None;
                if (body.streaming[0].state == "play" &&
                    body.vod[0].state == "stop")
                {
                    state = Streaming.Modes.Live;
                }
                else if (body.streaming[0].state == "stop" &&
                          body.vod[0].state == "play")
                {
                    state = Streaming.Modes.OnDemand;
                }
                else if (body.streaming[0].state == "play" &&
                  body.vod[0].state == "play")
                {
                    state = Streaming.Modes.All;
                }
                else if (body.streaming[0].state == "stop" &&
                         body.vod[0].state == "stop")
                {
                    state = Streaming.Modes.None;
                }
                else
                {
                    throw new System.Exception("Unknown state received: VOD = " + body.vod[0].state + ", Live = " + body.streaming[0].state);
                }

                // Change state
                this.streaming.Active = state;

                // If Live is active we must start encoder.
                if ((state & Streaming.Modes.Live) != 0)
                {
                    result = this.encoder.Start();
                    if (!result.Succeeded)
                    {
                        throw new System.Exception("wmEncoder.Start returns ERROR [" + result.Code + "]:" + result.Description);
                    }
                }
                else
                {
                    if (body.recording[0].state == "stop")
                    {
                        result = this.encoder.Stop();
                        if (!result.Succeeded)
                        {
                            throw new System.Exception("wmEncoder.Stop returns ERROR [" + result.Code + "]:" + result.Description);
                        }
                    }
                }

                // If OnDemand or Live is active we must start the streaming server.
                if (state != Streaming.Modes.None)
                {
                    bool allow_unicast = false;
                    if (System.Int32.Parse(this.dvrbConfig.output[0].GetliveRows()[0].allow_unicast) > 0)
                    {
                        allow_unicast = true;
                    }

                    if ((state & Streaming.Modes.Live) != 0)
                    {
                        result = this.streaming.SetLiveStreamingInfo(
                            internalAddr, 
                            internalPort, 
                            allow_unicast,
                            this.dvrbConfig.output[0].GetliveRows()[0].mgroup,
                            System.Int32.Parse(this.dvrbConfig.output[0].GetliveRows()[0].port));

                        if (!result.Succeeded)
                        {
                            throw new System.Exception("m_Streaming.SetLiveStreamingInfo ERROR: " + result.Description);
                        }
                    }

                    result = this.streaming.Start();
                    if (!result.Succeeded)
                    {
                        throw new System.Exception("m_Streaming.Start returns ERROR [" + result.Code + "]:" + result.Description);
                    }
                }
                else
                {
                    result = this.streaming.Stop();
                    if (!result.Succeeded)
                    {
                        throw new System.Exception("m_Streaming.Stop returns ERROR [" + result.Code + "]:" + result.Description);
                    }
                }

                this.SendResponse(200, "OK", response);
            }
            catch (System.Exception e)
            {
                log.Error("updateState:: Bad request from " + context.Request.UserHostAddress + ", exception :" + e.Message);
                this.SendResponse(400, "Bad Request: " + e.Message, response);
            }
        }

        private void GetState(Uri uri, string method, string content, HttpListenerContext context)
        {
            HttpListenerResponse response = context.Response;

            XmlSchemas.server_state body = new XmlSchemas.server_state();

            // Get recording state
            WMEncoderLib.WMENC_ARCHIVE_STATE record_state = this.encoder.RecordingState;
            switch (record_state)
            {
                case WMEncoderLib.WMENC_ARCHIVE_STATE.WMENC_ARCHIVE_RUNNING:
                    body.recording.AddrecordingRow("play"); 
                    break;
                case WMEncoderLib.WMENC_ARCHIVE_STATE.WMENC_ARCHIVE_PAUSED:
                    body.recording.AddrecordingRow("pause"); 
                    break;
                case WMEncoderLib.WMENC_ARCHIVE_STATE.WMENC_ARCHIVE_STOPPED:
                    body.recording.AddrecordingRow("stop"); 
                    break;
            }

            // get Live state
            if (this.streaming.LiveStreamingState == Streaming.State.Running)
            {
                body.streaming.AddstreamingRow("play");
            }
            else
            {
                body.streaming.AddstreamingRow("stop");
            }

            // get OnDemand state
            if (this.streaming.OnDemandState == Streaming.State.Running)
            {
                body.vod.AddvodRow("play");
            }
            else
            {
                body.vod.AddvodRow("stop");
            }

            log.Debug("getState:: Recording --> " + body.recording[0].state);
            log.Debug("getState:: Streaming --> " + body.streaming[0].state);
            log.Debug("getState:: vod --> " + body.vod[0].state);

            StringWriter sw = new StringWriter();
            body.WriteXml(sw);
            string xml = sw.ToString();

            this.SendResponse(xml, "application/xml", 200, "OK", response);
        }

        private void UpdateDVRB(Uri uri, string method, string content, HttpListenerContext context)
        {
            DVRBResult result;
            HttpListenerResponse response = context.Response;

            try
            {
                // parse xml body
                XmlSchemas.dvrb body = new XmlSchemas.dvrb();
                body.ReadXml(new System.IO.StringReader(content));
                log.Debug(content);

                Encoder.CodecType enc_type = Encoder.CodecType.CBR;
                switch (body.encoding[0].GetvideoRows()[0].GetvcodecRows()[0].type)
                {
                    case "CBR":
                        enc_type = Encoder.CodecType.CBR; 
                        break;
                    case "VBR_QUALITY":
                        enc_type = Encoder.CodecType.VBR_QUALITY; 
                        break;
                    case "VBR_BITRATE":
                        enc_type = Encoder.CodecType.VBR_BITRATE; 
                        break;
                    case "VBR_PEAK":
                        enc_type = Encoder.CodecType.VBR_PEAK; 
                        break;
                }

                if (body.encoding[0].GetvideoRows()[0].GetvcodecRows()[0].bitrate.Length == 0)
                {
                    body.encoding[0].GetvideoRows()[0].GetvcodecRows()[0].bitrate = "0";
                }

                if (body.encoding[0].GetvideoRows()[0].GetvcodecRows()[0].quality.Length == 0)
                {
                    body.encoding[0].GetvideoRows()[0].GetvcodecRows()[0].quality = "0";
                }

                if (body.encoding[0].GetvideoRows()[0].GetvcodecRows()[0].peak.Length == 0)
                {
                    body.encoding[0].GetvideoRows()[0].GetvcodecRows()[0].peak = "0";
                }

                // Set video props.
                result = this.encoder.SetVideoParams(
                                         System.Int32.Parse(body.encoding[0].GetvideoRows()[0].width),
                                         System.Int32.Parse(body.encoding[0].GetvideoRows()[0].height),
                                         System.Int32.Parse(body.encoding[0].GetvideoRows()[0].fps),
                                         enc_type,
                                         System.Int32.Parse(body.encoding[0].GetvideoRows()[0].GetvcodecRows()[0].bitrate),
                                         System.Int32.Parse(body.encoding[0].GetvideoRows()[0].GetvcodecRows()[0].quality),
                                         System.Int32.Parse(body.encoding[0].GetvideoRows()[0].GetvcodecRows()[0].peak),
                                         System.UInt64.Parse(body.encoding[0].GetvideoRows()[0].GetvcodecRows()[0].keyframe));

                if (!result.Succeeded)
                {
                    throw new System.Exception("wmEncoder.SetVideoParams ERROR: " + result.Description);
                }

                // Set audio props.
                result = this.encoder.SetAudioParams(
                    System.Int16.Parse(body.encoding[0].GetaudioRows()[0].channels),
                    System.Int32.Parse(body.encoding[0].GetaudioRows()[0].freq),
                    System.Int32.Parse(body.encoding[0].GetaudioRows()[0].GetacodecRows()[0].bitrate),
                    System.Int16.Parse(body.encoding[0].GetaudioRows()[0].bits));

                if (!result.Succeeded)
                {
                    throw new System.Exception("wmEncoder.SetAudioParams ERROR: " + result.Description);
                }

                // Set metadata
                result = this.encoder.SetMetadata(
                    body.metadata[0].title,
                    body.metadata[0].subject,
                    body.metadata[0].description,
                    body.metadata[0].source,
                    body.metadata[0].language,
                    body.metadata[0].relation,
                    body.metadata[0].coverage,
                    body.metadata[0].creator,
                    body.metadata[0].publisher,
                    body.metadata[0].contributor,
                    body.metadata[0].rights,
                    body.metadata[0].date,
                    body.metadata[0].type,
                    body.metadata[0].format,
                    body.metadata[0].id);

                if (!result.Succeeded)
                {
                    throw new System.Exception("wmEncoder.SetMetadata ERROR: " + result.Description);
                }

                // Set recording params
                if (body.file[0].name.Length > 0 && body.file[0].path.Length > 0 && body.file[0].interval.Length > 0)
                {
                    result = this.encoder.SetRecordParams(body.file[0].name, body.file[0].path, System.Int32.Parse(body.file[0].interval));
                    if (!result.Succeeded)
                    {
                        throw new System.Exception("wmEncoder.SetRecordParams ERROR: " + result.Description);
                    }
                }

                // Set source params
                foreach (XmlSchemas.dvrb.inputRow input in body.input)
                {
                    Encoder.ESourceType source_type;
                    switch (input.type)
                    {
                        case "video":
                            source_type = Encoder.ESourceType.DeviceVideo; 
                            break;
                        case "audio":
                            source_type = Encoder.ESourceType.DeviceAudio; 
                            break;
                        case "desktop":
                            source_type = Encoder.ESourceType.DesktopVideo; 
                            break;
                        default:
                            throw new System.Exception("Unknown source type " + input.type);
                    }

                    result = this.encoder.SetSource(source_type, input.name);
                    if (!result.Succeeded)
                    {
                        throw new System.Exception("wmEncoder.SetSource ERROR: " + result.Description);
                    }
                }

                // Set live streaming params                
                result = this.encoder.SetOutParams(internalPort);
                if (!result.Succeeded)
                {
                    throw new System.Exception("wmEncoder.SetOutParams ERROR: " + result.Description);
                }

                // Set OnDemand params
                if (body.output[0].GetvodRows()[0].path.Length > 0)
                {
                    result = this.streaming.SetVoDInfo(body.output[0].GetvodRows()[0].path);
                    if (!result.Succeeded)
                    {
                        throw new System.Exception("m_Streaming.SetVoDInfo ERROR: " + result.Description);
                    }
                }

                this.dvrbConfig = body;

                this.SendResponse(200, "OK", response);
            }
            catch (System.Exception e)
            {
                log.Error("updateDVRB:: Bad request from " + context.Request.UserHostAddress + ", exception :" + e.Message);
                this.SendResponse(400, "Bad Request: " + e.Message, response);
            }
        }

        private void GetDVRB(Uri uri, string method, string content, HttpListenerContext context)
        {
            HttpListenerResponse response = context.Response;

            if (this.dvrbConfig != null)
            {
                StringWriter sw = new StringWriter();
                this.dvrbConfig.WriteXml(sw);
                string xml = sw.ToString();

                this.SendResponse(xml, "application/xml", 200, "OK", response);
            }
            else
            {
                log.Error("getDVRB:: Bad request from " + context.Request.UserHostAddress + ". System not configured.");
                this.SendResponse(400, "Bad Request: System not configured.", response);
            }
        }

        private void GetSystemInfo(Uri uri, string method, string content, HttpListenerContext context)
        {
            string responseString = SystemInfo.GetSystemInfo();
            this.SendResponse(responseString, "application/xml", 200, "OK", context.Response);
        }

        private void GetInfo(Uri uri, string method, string content, HttpListenerContext context)
        {
            this.SendResponse(501, "Not Implemented", context.Response);
        }

        private void NotFound(Uri uri, string method, string content, HttpListenerContext context)
        {
            HttpListenerResponse response = context.Response;
            string responseText = "Not Found";
            string responseString = "<html><body>" + "\n" + responseText + "</body></html>";

            this.SendResponse(responseString, "text/html", 404, responseText, response);
        }

        private void NotAllowed(Uri uri, string method, string content, HttpListenerContext context)
        {
            HttpListenerResponse response = context.Response;
            string responseText = "Not Allowed";
            string responseString = "<html><body>" + "\n" + responseText + "</body></html>";

            this.SendResponse(responseString, "text/html", 405, responseText, response);
        }

        private void SendResponse(string responseContent, string responseContentType, int statusCode, string statusDescription, HttpListenerResponse response)
        {
            response.StatusCode = statusCode;
            response.StatusDescription = statusDescription;

            if (responseContentType != null)
            {
                response.ContentType = responseContentType;
            }

            if (responseContent != null)
            {
                byte[] buffer = System.Text.Encoding.UTF8.GetBytes(responseContent);
                response.ContentLength64 = buffer.Length;
                response.OutputStream.Write(buffer, 0, buffer.Length);
            }

            response.OutputStream.Close();
        }

        private void SendResponse(int statusCode, string statusDescription, HttpListenerResponse response)
        {
            this.SendResponse(statusCode, statusDescription, response);
        }

        private IList<string> SplitUri(Uri uri)
        {
            string path = uri.AbsolutePath;

            if (path.EndsWith("/"))
            {
                path = path.Remove(path.Length - 1);
            }

            return path.Split('/');
        }
    }
}
