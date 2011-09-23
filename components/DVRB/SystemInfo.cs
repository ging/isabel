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
    using System.Xml;
    using NLog;

    internal class SystemInfo
    {
        private const string OsLabel = "os";
        private const string RootLabel = "sytem";
        private const string DriveLabel = "drive";
        private const string SourceLabel = "source";
        private const string RamLabel = "ram";
        private const string PhysicalLabel = "physical";
        private const string VirtualLabel = "virtual";

        private static Logger log = LogManager.GetCurrentClassLogger();

        private static Microsoft.VisualBasic.Devices.ComputerInfo computer = new Microsoft.VisualBasic.Devices.ComputerInfo();

        public static string GetSystemInfo()
        {
            System.Text.StringBuilder text = new System.Text.StringBuilder();
            XmlWriter writer = XmlWriter.Create(text);
            writer.WriteStartDocument();
            writer.WriteComment("General system information.");
            writer.WriteStartElement(RootLabel);
            FillOS(writer);
            FillDrives(writer);
            FillSources(writer);
            FillRam(writer);
            writer.WriteEndElement(); 
            writer.WriteEndDocument();
            writer.Flush();
            log.Trace(text);
            return text.ToString();
        }

        private static void AddSources(XmlWriter writer, System.Collections.Generic.IList<string> sources, string type)
        {
            foreach (string source in sources)
            {
                writer.WriteStartElement(SourceLabel);
                writer.WriteAttributeString("type", type);
                writer.WriteAttributeString("name", source.ToString());
                writer.WriteAttributeString("port", string.Empty);
                writer.WriteEndElement();
            }
        }

        private static void FillOS(XmlWriter writer)
        {
            writer.WriteStartElement(OsLabel);
            writer.WriteAttributeString("platform", computer.OSPlatform);
            writer.WriteAttributeString("name", computer.OSFullName);
            writer.WriteAttributeString("version", computer.OSVersion);
            writer.WriteEndElement();
        }

        private static void FillDrives(XmlWriter writer)
        {
            System.IO.DriveInfo[] drives = System.IO.DriveInfo.GetDrives();
            foreach (System.IO.DriveInfo drive in drives)
            {
                if (drive.IsReady)
                {
                    writer.WriteStartElement(DriveLabel);
                    writer.WriteAttributeString("name", drive.Name);
                    writer.WriteAttributeString("label", drive.VolumeLabel);
                    writer.WriteAttributeString("type", drive.DriveType.ToString());
                    writer.WriteAttributeString("format", drive.DriveFormat);
                    writer.WriteAttributeString("total_size", drive.TotalSize.ToString());
                    writer.WriteAttributeString("free_space", drive.AvailableFreeSpace.ToString());
                    writer.WriteEndElement();
                }
            }
        }

        private static void FillRam(XmlWriter writer)
        {
            writer.WriteStartElement(RamLabel);
            writer.WriteStartElement(PhysicalLabel);
            writer.WriteAttributeString("available", computer.AvailablePhysicalMemory.ToString());
            writer.WriteAttributeString("total", computer.TotalPhysicalMemory.ToString());
            writer.WriteEndElement();
            writer.WriteStartElement(VirtualLabel);
            writer.WriteAttributeString("available", computer.AvailableVirtualMemory.ToString());
            writer.WriteAttributeString("total", computer.TotalVirtualMemory.ToString());
            writer.WriteEndElement();
            writer.WriteEndElement();
        }

        private static void FillSources(XmlWriter writer)
        {
            System.Collections.Generic.IList<string> sources;
            sources = Encoder.GetSources(Encoder.ESourceType.DeviceVideo);
            AddSources(writer, sources, "video");
            sources = Encoder.GetSources(Encoder.ESourceType.DeviceAudio);
            AddSources(writer, sources, "audio");
            sources = Encoder.GetSources(Encoder.ESourceType.DesktopVideo);
            AddSources(writer, sources, "desktop");
        }
    }
}
