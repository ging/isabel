namespace DVRB
{
    using System;
    using System.Collections.Generic;
    using System.ComponentModel;
    using System.Configuration.Install;
    using System.Text;

    [RunInstaller(true)]
    public class ProjectInstaller : Installer
    {
        private System.ServiceProcess.ServiceProcessInstaller serviceProcessInstaller1;
        private System.ServiceProcess.ServiceInstaller serviceInstaller1;

        public ProjectInstaller()
        {
            this.serviceProcessInstaller1 = new System.ServiceProcess.ServiceProcessInstaller();
            this.serviceInstaller1 = new System.ServiceProcess.ServiceInstaller();
             
            // serviceProcessInstaller1
            this.serviceProcessInstaller1.Account = System.ServiceProcess.ServiceAccount.LocalSystem;
            this.serviceProcessInstaller1.Password = null;
            this.serviceProcessInstaller1.Username = null;
             
            // serviceInstaller1
            this.serviceInstaller1.ServiceName = "DVRB";
            this.serviceInstaller1.Description = "DVRB (aka Double Vodka with Red Bull)";
             
            // ProjectInstaller 
            this.Installers.AddRange(
                new System.Configuration.Install.Installer[] { this.serviceProcessInstaller1, this.serviceInstaller1 });
        }
    }
}
