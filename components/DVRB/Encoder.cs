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
    using NLog;
    using WMEncoderLib;

    /**
     * Class wrapper that manages Windows Media Encoder application.
     */
    internal class Encoder
    {
        /**
         * Waiting event time out. 
         */
        private static readonly int startStopWaitTimeout = 10000;

        /**
         * Archive waiting event time out. 
         */
        private static readonly int archiveWaitTimeout = System.Threading.Timeout.Infinite;

        /**
         * Max stop recording retries.
         */
        private const int MaxRetries = 30;

        /**
         * Nlog class.
         */
        private static Logger log = LogManager.GetCurrentClassLogger();

        /**
         * Enable/disable recording.
         */
        private bool enableRecording;

        /**
         * Recording path.
         */
        private string recordingPath;

        /**
         * Max record time (in seconds).
         */
        private int chopLength;

        /**
         * Record fileName index.
         */
        private int chopNumber;

        /**
         * Record fileName index.
         */
        private string fileNamePrefix;

        /**
         * Recording chopper timer.
         */
        private System.Timers.Timer choppingTimer;

        /**
         * Windows Media Encoder object.
         */
        private WMEncoder wmEncoder;

        /**
         * Interface to source group, a source group is composed by video/audio sources.
         */
        private IWMEncSourceGroup2 sourceGroup;

        /**
         * Interface to the audio source.
         */
        private IWMEncAudioSource audioSource;

        /**
         * Interface to the video source.
         */
        private IWMEncVideoSource videoSource;

        /**
         * Interface to encoding profile.
         */
        private IWMEncProfile2 wmEncoderProfile;

        /**
         * Interface to the particular conding parameters for the audience.
         */
        private IWMEncAudienceObj wmEncoderAudience;

        /**
         * State changed event.
         */
        private System.Threading.EventWaitHandle stateChangeWaitHandle;

        /**
         * Archive state changed event.
         */
        private System.Threading.EventWaitHandle archiveStateChangeWaitHandle;

        /**
         * The archive state.
         */
        private WMENC_ARCHIVE_STATE archiveState;
        
        /**
         * Constructor.
         */
        public Encoder()
        {
            this.enableRecording = false;
            this.chopNumber = 0;
            this.chopLength = -1;

            this.choppingTimer = new System.Timers.Timer();
            this.choppingTimer.Elapsed += this.OnRecordTimeElapsed;
            this.archiveState = WMENC_ARCHIVE_STATE.WMENC_ARCHIVE_STOPPED;
            this.stateChangeWaitHandle = new System.Threading.EventWaitHandle(false, System.Threading.EventResetMode.ManualReset, string.Empty);
            this.archiveStateChangeWaitHandle = new System.Threading.EventWaitHandle(false, System.Threading.EventResetMode.ManualReset, string.Empty);

            // Audio and video sources
            this.audioSource = null;
            this.videoSource = null;

            // Windows Media Encoder
            this.wmEncoder = new WMEncoderClass();
            this.sourceGroup = (IWMEncSourceGroup2)this.wmEncoder.SourceGroupCollection.Add("SG_1");
            this.wmEncoderProfile = new WMEncProfile2();

            // For now, we only listen these events
            this.wmEncoder.OnStateChange += this.OnStateChange;
            this.wmEncoder.OnError += this.OnError;
            this.wmEncoder.OnArchiveStateChange += this.OnArchiveStateChange;

            // Set recording props.
            this.wmEncoder.EnableAutoArchive = false;
            this.wmEncoder.AutoIndex = false;

            // Set default profile values                        
            this.wmEncoderProfile.ValidateMode = true;
            this.wmEncoderProfile.ProfileName = "Windows Media Encoder Profile";
            this.wmEncoderProfile.ContentType = 17; // Audio + Video
            this.wmEncoderAudience = this.wmEncoderProfile.AddAudience(100000); // Initial bitrate = 100 kbps, will change automatically 
            // when user sets video and audio params.
        }

        /**
        * Destructor.
        */
        ~Encoder()
        {
            this.wmEncoder.Stop();
        }

        /**
         * Enumerates available source types:
         * - DeviceAudio : Represents an audio device like soundcard.
         * - DeviceVideo : Represents a video device like a webcam.
         * - DesktopVideo : Represents screen capture source.
         */
        public enum ESourceType : ulong
        {
            DeviceAudio = 0x01,
            DeviceVideo = 0x02,
            DesktopVideo = 0x03
        }

        /**
         * Enumerates encoding type:
         * - CBR : Constant bit rate. When using CBR only bitrate param applies.
         * - VBR_QUALITY : Quality based VBR mode.
         * - VBR_BITRATE : Bit-rate based VBR mode.
         * - VBR_PEAK : Constrained VBR mode is used. 'peak' param is applied.
         */
        public enum CodecType : ulong
        {
            CBR = 0x01,
            VBR_QUALITY = 0x02,
            VBR_BITRATE = 0x03,
            VBR_PEAK = 0x04
        }

        /**
        * Property to retrieve the encoder state.
        * @return Encoder state.
        */
        public WMENC_ARCHIVE_STATE RecordingState
        {
            get { return this.archiveState; }
        }

        /**
         * Property to retrieve the encoder state.
         * @return Encoder state.
         */
        public WMENC_ENCODER_STATE CurrentState
        {
            get { return this.wmEncoder.RunState; }
        }

        /**
         * Get available sources of specified type.
         * @param type : Source type.
         * @return list of available sources. This sources can be selected by SetSource(...) method.
         */
        public static IList<string> GetSources(ESourceType type)
        {
            System.Collections.Generic.IList<string> sources = new System.Collections.Generic.List<string>();
            try
            {
                WMEncoderClass encoder = new WMEncoderClass();
                IWMEncSourcePluginInfoManager srcPlugMgr = encoder.SourcePluginInfoManager;
                IWMEncPluginInfo plugInfo;

                for (int i = 0; i < srcPlugMgr.Count; i++)
                {
                    plugInfo = srcPlugMgr.Item(i);
                    if (type == ESourceType.DesktopVideo &&
                        plugInfo.SchemeType == "ScreenCap" &&
                        plugInfo.MediaType == WMENC_SOURCE_TYPE.WMENC_VIDEO)
                    {
                        log.Debug(type + " source -->" + plugInfo.Name);
                        sources.Add(plugInfo.Name);
                    }
                    else
                    {
                        if (plugInfo.SchemeType == "DEVICE" && plugInfo.Resources == true)
                        {
                            for (int j = 0; j < plugInfo.Count; j++)
                            {
                                if (plugInfo.MediaType == WMENC_SOURCE_TYPE.WMENC_AUDIO &&
                                    type == ESourceType.DeviceAudio)
                                {
                                    log.Debug(type + " source(" + j + ") -->" + plugInfo.Item(j));
                                    sources.Add(plugInfo.Item(j));
                                }

                                if (plugInfo.MediaType == WMENC_SOURCE_TYPE.WMENC_VIDEO &&
                                    type == ESourceType.DeviceVideo)
                                {
                                    log.Debug(type + " source(" + j + ") -->" + plugInfo.Item(j));
                                    sources.Add(plugInfo.Item(j));
                                }
                            }
                        }
                    }
                }
            }
            catch (Exception e)
            {
                log.ErrorException("Exception retrieving sources", e);
                sources = null;
            }

            return sources;
        }

        /**
         * Enable/disable recording.
         */
        public DVRBResult EnableRecording(bool enable)
        {
            DVRBResult result = new DVRBResult();

            this.enableRecording = enable; // Save state
            if (this.enableRecording) // If enabled
            {
                if (this.archiveState != WMENC_ARCHIVE_STATE.WMENC_ARCHIVE_RUNNING) // if not running
                {
                    this.Start(); // Start running
                    
                    string fileName = this.recordingPath + this.fileNamePrefix;
                    if (this.chopLength > 0) // if chopping
                    {
                        fileName += "_" + this.chopNumber;
                    }

                    result = this.StartRecording(fileName + ".wmv"); // Start recording
                }

                if (this.chopLength > 0) // Start chopper timer
                {
                    this.choppingTimer.Interval = this.chopLength * 1000;
                    this.choppingTimer.Start();
                }
            }
            else
            {
                this.choppingTimer.Stop(); // Stop chopper timer
                result = this.StopRecording(); // Stop recording
            }

            return result;
        }

        /**
         * Start encoding session. Make sure you have propperly configured the session before calling this method:
         *  - Configure input with 'SetSource(...)'.
         *  - Configure video and audio encoding params with 'SetVideoParams(...)' and 'SetAudioParams(...)'.
         *  - Configure output port with 'SetOutputPort(...)'
         * @return Method result.
         */
        public DVRBResult Start()
        {
            DVRBResult result = new DVRBResult();
            log.Debug("Start encoding on port " + this.wmEncoder.Broadcast.get_PortNumber(WMENC_BROADCAST_PROTOCOL.WMENC_PROTOCOL_HTTP));
            try
            {
                if (this.wmEncoder.RunState != WMENC_ENCODER_STATE.WMENC_ENCODER_RUNNING)
                {
                    this.sourceGroup.set_Profile(this.wmEncoderProfile);
                    this.wmEncoder.PrepareToEncode(true);
                    this.wmEncoder.Start();
                    if (this.stateChangeWaitHandle.WaitOne(startStopWaitTimeout, true) == false)
                    {
                        log.Error("Start timeout");
                        result = new DVRBResult(DVRBResult.ERROR_TIMEOUT, DVRBResult.ERROR_TIMEOUT_DESC);
                    }
                    else if (this.wmEncoder.RunState != WMENC_ENCODER_STATE.WMENC_ENCODER_RUNNING)
                    {
                        log.Error("Start state != WMENC_ENCODER_RUNNING");
                        result = new DVRBResult(DVRBResult.ERROR, DVRBResult.ERROR_DESC);
                    }
                }
                else
                {
                    log.Debug("Encoding already running.");
                }
            }
            catch (Exception e)
            {
                log.ErrorException("Start encoding exception:" + e.Message, e);
            }

            return result;
        }

        /**
         * Stops the encoding session.
         * @return Method result.
         */
        public DVRBResult Stop()
        {
            DVRBResult result = new DVRBResult();
            log.Debug("Stop encoding");
            try
            {
                if (this.wmEncoder.RunState != WMENC_ENCODER_STATE.WMENC_ENCODER_STOPPED)
                {
                    this.wmEncoder.Stop();
                    if (this.stateChangeWaitHandle.WaitOne(startStopWaitTimeout, true) == false)
                    {
                        log.Error("Stop timeout");
                        result = new DVRBResult(DVRBResult.ERROR_TIMEOUT, DVRBResult.ERROR_TIMEOUT_DESC);
                    }
                    else if (this.wmEncoder.RunState != WMENC_ENCODER_STATE.WMENC_ENCODER_STOPPED)
                    {
                        log.Error("Stop state != WMENC_ENCODER_STOPPED");
                        result = new DVRBResult(DVRBResult.ERROR, DVRBResult.ERROR_DESC);
                    }
                }
                else
                {
                    log.Debug("Encoding already stopped.");
                }
            }
            catch (Exception e)
            {
                log.ErrorException("Stop encoding exception:" + e.Message, e);
            }

            return result;
        }

        /**
         *  Title: String.
         *  Subject: String.
         *  Description: String.
         *  Source: String.
         *  Language: String.
         *  Relation: String.
         *  Coverage: String.
         *  Creator: String.
         *  Publisher: String.
         *  Contributor: String.
         *  Rights: String.
         *  Date: String.
         *  Type: String.
         *  Format: String.
         *  Identifier: String.
         */
        public DVRBResult SetMetadata(
                                string title,
                                string subject,
                                string description,
                                string source,
                                string language,
                                string relation,
                                string coverage,
                                string creator,
                                string publisher,
                                string contributor,
                                string rights,
                                string date,
                                string type,
                                string format,
                                string identifier)
        {
            // Fill in the description object members.
            IWMEncDisplayInfo descr = this.wmEncoder.DisplayInfo;
            descr.Title = title;
            descr.Description = description;
            descr.Author = creator;
            descr.Copyright = rights;

            // Add an attribute to the collection.
            IWMEncAttributes attr = this.wmEncoder.Attributes;
            attr.RemoveAll();
            attr.Add("title", title);
            attr.Add("subject", subject);
            attr.Add("description", description);
            attr.Add("source", source);
            attr.Add("language", language);
            attr.Add("relation", relation);
            attr.Add("coverage", coverage);
            attr.Add("creator", creator);
            attr.Add("publisher", publisher);
            attr.Add("contributor", contributor);
            attr.Add("rights", rights);
            attr.Add("date", date);
            attr.Add("type", type);
            attr.Add("format", format);
            attr.Add("identifier", identifier);
            return new DVRBResult();
        }

        /**
         * Sets output port where http streaming protocol will be available.
         * Windows Media Service or Windows Media Player can connect to the encoding session
         * throught "http://host:port/".
         * @param port
         */
        public DVRBResult SetOutParams(int port)
        {
            try
            {
                // Set the port number.
                log.Debug("Setting pull port = " + port);
                this.wmEncoder.Broadcast.set_PortNumber(WMENC_BROADCAST_PROTOCOL.WMENC_PROTOCOL_HTTP, port);
            }
            catch (Exception e)
            {
                log.ErrorException("Exception setting out port: " + e.Message, e);
                return new DVRBResult(DVRBResult.ERROR, e.Message);
            }

            return new DVRBResult();
        }

        /**
         * Sets the recording params.
         */
        public DVRBResult SetRecordParams(string fileNamePrefix, string path, int chopLength)
        {
            this.fileNamePrefix = fileNamePrefix;
            this.recordingPath = path;
            this.chopLength = chopLength;
            this.chopNumber = 0;
            if (!this.recordingPath.EndsWith("\\"))
            {
                this.recordingPath += "\\";
            }

            // Create dirs...
            try
            {
                int start = this.recordingPath.IndexOf("\\");
                while (start > 0)
                {
                    int end = this.recordingPath.IndexOf("\\", start + 1);
                    string dir = this.recordingPath.Substring(0, end);
                    System.IO.Directory.CreateDirectory(dir);
                    start = end;
                }
            }
            catch (Exception)
            {
            }

            return new DVRBResult();
        }

        /**
         * Sets the video encoding parameters.
         * @param width : Video width.
         * @param height : Video height.
         * @param fps : Video frames per second.
         * @param type : Enumerator with encoding types (see CodecType doc.).
         * @param bitrate : Video bitrate, only applies when type = CBR or type = VBR_BITRATE.
         * @param quality : Video quality, only applies when type = VBR_QUALITY.
         * @param peak : Video bandwidth peak, only applies when type = VBR_PEAK.
         * @param key_interval : Intervalo de tiempo entre "key frames" (en segundos)
         */
        public DVRBResult SetVideoParams(
                                   int width,
                                   int height,
                                   int fps,
                                   CodecType type,
                                   int bitrate,
                                   int quality,
                                   int peak,
                                   ulong key_interval)
        {
            try
            {
                this.wmEncoderAudience.set_VideoWidth(0, width);
                this.wmEncoderAudience.set_VideoHeight(0, height);
                this.wmEncoderAudience.set_VideoFPS(0, fps * 1000);
                this.wmEncoderAudience.set_VideoKeyFrameDistance(0, key_interval * 1000);

                switch (type)
                {
                    case CodecType.CBR:
                        this.wmEncoderProfile.set_VBRMode(WMENC_SOURCE_TYPE.WMENC_VIDEO, 0, WMENC_PROFILE_VBR_MODE.WMENC_PVM_NONE);
                        this.wmEncoderAudience.set_VideoBitrate(0, bitrate);
                        break;
                    case CodecType.VBR_QUALITY:
                        this.wmEncoderProfile.set_VBRMode(WMENC_SOURCE_TYPE.WMENC_VIDEO, 0, WMENC_PROFILE_VBR_MODE.WMENC_PVM_UNCONSTRAINED);
                        this.wmEncoderAudience.set_VideoCompressionQuality(0, quality);
                        break;
                    case CodecType.VBR_BITRATE:
                        this.wmEncoderProfile.set_VBRMode(WMENC_SOURCE_TYPE.WMENC_VIDEO, 0, WMENC_PROFILE_VBR_MODE.WMENC_PVM_BITRATE_BASED);
                        this.wmEncoderAudience.set_VideoBitrate(0, bitrate);
                        break;
                    case CodecType.VBR_PEAK:
                        this.wmEncoderProfile.set_VBRMode(WMENC_SOURCE_TYPE.WMENC_VIDEO, 0, WMENC_PROFILE_VBR_MODE.WMENC_PVM_PEAK);
                        this.wmEncoderAudience.set_VideoPeakBitrate(0, peak);
                        break;
                }
            }
            catch (Exception e)
            {
                return new DVRBResult(DVRBResult.ERROR, e.Message);
            }

            return new DVRBResult();
        }

        /**
        * Sets the audio encoding parameters.
        * @param nchannels : 1 == mono, 2 == stereo.
        * @param freq : Samples per second.
        * @param bitrate : Codec bit-rate.
        * @param bits : Bits per sample (typical values are 8 and 16).
        */
        public DVRBResult SetAudioParams(short nchannels, int freq, int bitrate, short bits)
        {
            try
            {
                this.wmEncoderAudience.SetAudioConfig(0, nchannels, freq, bitrate, bits);
            }
            catch (Exception e)
            {
                return new DVRBResult(DVRBResult.ERROR, e.Message);
            }

            return new DVRBResult();
        }

        /**
         * Set source.
         * @param type : source type.
         * @param name : source name.
         */
        public DVRBResult SetSource(ESourceType type, string name)
        {
            try
            {                
                string scheme = string.Empty;
                IWMEncSource source = null;
                switch (type)
                {
                    case ESourceType.DeviceAudio:                        
                        scheme = "Device";
                        if (this.audioSource == null)
                        {
                            this.audioSource = (IWMEncAudioSource)this.sourceGroup.AddSource(WMENC_SOURCE_TYPE.WMENC_AUDIO);
                        }

                        source = this.audioSource;
                        break;
                    case ESourceType.DeviceVideo:                        
                        scheme = "Device";
                        if (this.videoSource == null)
                        {
                            this.videoSource = (IWMEncVideoSource)this.sourceGroup.AddSource(WMENC_SOURCE_TYPE.WMENC_VIDEO);
                        }

                        source = this.videoSource;
                        break;
                    case ESourceType.DesktopVideo:
                        scheme = "ScreenCap";
                        if (this.videoSource == null)
                        {
                            this.videoSource = (IWMEncVideoSource)this.sourceGroup.AddSource(WMENC_SOURCE_TYPE.WMENC_VIDEO);
                        }

                        source = this.videoSource;
                        break;
                }

                log.Debug("Setting " + scheme + " -->" + name);
                source.SetInput(name, scheme, string.Empty);
            }
            catch (Exception e)
            {
                return new DVRBResult(DVRBResult.ERROR, e.Message);
            }

            return new DVRBResult();
        }

        /**
 * Method to manage error events.
 */
        private void OnError(int error)
        {
            log.Error("OnError: " + error);
        }

        /**
         * Method to manage state changes events.
         */
        private void OnRecordTimeElapsed(object sender, System.Timers.ElapsedEventArgs args)
        {
            log.Trace("OnRecordTimeElapsed :: Time to change fileName record.");
            this.chopNumber++;
            if (this.enableRecording)
            {
                if (this.recordingPath.Length > 0 && this.fileNamePrefix.Length > 0)
                {
                    this.StartRecording(this.recordingPath + this.fileNamePrefix + "_" + this.chopNumber + ".wmv");
                }
            }
            else
            {
                this.choppingTimer.Stop();
            }
        }

        /**
         * Method to manage state changes events.
         */
        private void OnStateChange(WMENC_ENCODER_STATE enumState)
        {
            switch (enumState)
            {
                case WMENC_ENCODER_STATE.WMENC_ENCODER_RUNNING:
                    log.Debug("OnStateChange --> WMENC_ENCODER_RUNNING");
                    break;

                case WMENC_ENCODER_STATE.WMENC_ENCODER_STOPPED:
                    log.Debug("OnStateChange --> WMENC_ENCODER_STOPPED");
                    break;

                case WMENC_ENCODER_STATE.WMENC_ENCODER_STARTING:
                    log.Debug("OnStateChange --> WMENC_ENCODER_STARTING");
                    break;

                case WMENC_ENCODER_STATE.WMENC_ENCODER_PAUSING:
                    log.Debug("OnStateChange --> WMENC_ENCODER_PAUSING");
                    break;

                case WMENC_ENCODER_STATE.WMENC_ENCODER_STOPPING:
                    log.Debug("OnStateChange --> WMENC_ENCODER_STOPPING");
                    break;

                case WMENC_ENCODER_STATE.WMENC_ENCODER_PAUSED:
                    log.Debug("OnStateChange --> WMENC_ENCODER_PAUSED");
                    break;

                case WMENC_ENCODER_STATE.WMENC_ENCODER_END_PREPROCESS:
                    log.Debug("OnStateChange --> WMENC_ENCODER_END_PREPROCESS");
                    break;
            }

            // Signals that the state has changed
            this.stateChangeWaitHandle.Set();
        }

        /**
         * Method to manage state changes events.
         */
        private void OnArchiveStateChange(WMENC_ARCHIVE_TYPE type, WMENC_ARCHIVE_STATE state)
        {
            switch (state)
            {
                case WMENC_ARCHIVE_STATE.WMENC_ARCHIVE_RUNNING:
                    log.Debug("OnStateChange --> WMENC_ARCHIVE_RUNNING");
                    break;

                case WMENC_ARCHIVE_STATE.WMENC_ARCHIVE_PAUSED:
                    log.Debug("OnStateChange --> WMENC_ARCHIVE_PAUSED");
                    break;

                case WMENC_ARCHIVE_STATE.WMENC_ARCHIVE_STOPPED:
                    log.Debug("OnStateChange --> WMENC_ARCHIVE_STOPPED");
                    break;
            }

            this.archiveState = state;
            this.archiveStateChangeWaitHandle.Set();
        }

        /**
         * Method to start recording process.
         * @param fileName : Full fileName name (path + fileName).
         */
        private DVRBResult StartRecording(string fileName)
        {
            DVRBResult result = new DVRBResult();

            if (fileName.Length > 0)
            {
                if (this.wmEncoder.RunState == WMENC_ENCODER_STATE.WMENC_ENCODER_RUNNING)
                {
                    if (this.archiveState != WMENC_ARCHIVE_STATE.WMENC_ARCHIVE_STOPPED)
                    {
                        this.StopRecording();
                    }

                    result = new DVRBResult(DVRBResult.ERROR, DVRBResult.ERROR_DESC);
                    for (int i = 0; i < MaxRetries && !result.Succeeded; i++)
                    {
                        try
                        {
                            log.Debug("[" + i + "] Setting recording fileName = " + fileName);
                            ((IWMEncFile2)this.wmEncoder.File).LocalFileName = fileName;
                        }
                        catch (Exception e)
                        {
                            log.ErrorException("Exception setting fileName params: " + e.Message, e);
                        }

                        this.archiveStateChangeWaitHandle.Reset();
                        log.Debug("[" + i + "] StartRecording starting record...");
                        this.wmEncoder.Archive(WMENC_ARCHIVE_TYPE.WMENC_ARCHIVE_LOCAL, WMENC_ARCHIVE_OPERATION.WMENC_ARCHIVE_START);
                        if (this.archiveStateChangeWaitHandle.WaitOne(archiveWaitTimeout, true) == false)
                        {
                            log.Error("[" + i + "] StartRecording timeout");
                            result = new DVRBResult(DVRBResult.ERROR_TIMEOUT, DVRBResult.ERROR_TIMEOUT_DESC);
                        }
                        else if (this.archiveState != WMENC_ARCHIVE_STATE.WMENC_ARCHIVE_RUNNING)
                        {
                            log.Error("[" + i + "] Archive state != WMENC_ARCHIVE_RUNNING");
                            result = new DVRBResult(DVRBResult.ERROR, DVRBResult.ERROR_DESC);
                        }
                        else
                        {
                            result = new DVRBResult();
                        }
                    }
                }
                else
                {
                    result = new DVRBResult(DVRBResult.ERROR, "Encoder is not running");
                }
            }
            else
            {
                result = new DVRBResult(DVRBResult.ERROR, "Recording is not configured");
            }

            return result;
        }

        /**
         * Method to stop recording process.
         */
        private DVRBResult StopRecording()
        {
            DVRBResult result = new DVRBResult();
            if (this.archiveState != WMENC_ARCHIVE_STATE.WMENC_ARCHIVE_STOPPED)
            {
                this.archiveStateChangeWaitHandle.Reset();
                result = new DVRBResult(DVRBResult.ERROR, DVRBResult.ERROR_DESC);
                for (int i = 0; i < MaxRetries && !result.Succeeded; i++)
                {
                    log.Debug("[" + i + "] StopRecording stopping record...");
                    this.wmEncoder.Archive(WMENC_ARCHIVE_TYPE.WMENC_ARCHIVE_LOCAL, WMENC_ARCHIVE_OPERATION.WMENC_ARCHIVE_STOP);
                    if (this.archiveStateChangeWaitHandle.WaitOne(archiveWaitTimeout, true) == false)
                    {
                        log.Error("[" + i + "] StopRecording timeout");
                        result = new DVRBResult(DVRBResult.ERROR_TIMEOUT, DVRBResult.ERROR_TIMEOUT_DESC);
                    }
                    else if (this.archiveState != WMENC_ARCHIVE_STATE.WMENC_ARCHIVE_STOPPED)
                    {
                        log.Error("[" + i + "] Archive state != WMENC_ARCHIVE_STOPPED");
                        result = new DVRBResult(DVRBResult.ERROR, DVRBResult.ERROR_DESC);
                    }
                    else
                    {
                        result = new DVRBResult();
                    }
                }
            }

            return result;
        }
    }
}
