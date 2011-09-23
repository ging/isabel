
namespace DVRB
{
    using System;
    using System.Collections.Generic;
    using System.ServiceProcess;
    using System.Text;
    using NLog;

    public class DVRBService : ServiceBase
    {
        private static Logger log = LogManager.GetCurrentClassLogger();

        private FlashPolicyServer policyServer;
        private Encoder encoder;
        private Streaming stream;
        private WebService webService;

        public DVRBService()
        {
            this.ServiceName = "DVRB";
        }

        // This method starts the service in interactive mode
        internal void DVRBStart(string[] args) 
        {
            this.OnStart(args);
        }

        // This method stops the service in interactive mode
        internal void DVRBStop()
        {
            this.OnStop();
        }

        // Service start
        protected override void OnStart(string[] args)
        {
            log.Trace("-------------------------------");
            log.Trace("DVRB STARTING APPLICATION...");
            log.Trace("-------------------------------");

            this.policyServer = new FlashPolicyServer();
            this.encoder = new Encoder();
            this.stream = new Streaming();
            this.webService = new WebService("http://+:8080/", this.encoder, this.stream);
            this.policyServer.Start();
            this.webService.StartWebServiceListener();
        }

        // Service stop
        protected override void OnStop()
        {
            this.policyServer.Stop();
            this.webService.Stop();
        }
    }
}
