using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using PowerPoint = Microsoft.Office.Interop.PowerPoint;
using Microsoft.Office.Core;
using System.Diagnostics;
using System.Net.Sockets;
using System.IO;


namespace PowerPointServer
{
    public partial class Form1 : Form
    {
        private static int LOCAL_PORT = 5000;
        private PowerPoint.Application oPPT;
        private StreamWriter stream;
        private TcpListener server;
        private TcpClient client;

        public Form1()
        {
            InitializeComponent();
        }

        private void NextClient()
        {
            stream = null;
            if (client != null)
                client.Close();
            server.BeginAcceptTcpClient(new AsyncCallback(InitClient), server);
        }

        private void InitClient(IAsyncResult result)
        {
            client = ((TcpListener)(result.AsyncState)).EndAcceptTcpClient(result);
            NetworkStream netStream = client.GetStream();
            stream = new StreamWriter(netStream);
            SetText("Client connected from " + client.Client.RemoteEndPoint);
            netStream.BeginRead(new byte[100], 0, 100, new AsyncCallback(ClientClosed), null);
        }

        private void ClientClosed(IAsyncResult result)
        {
            Debug.WriteLine("CLIENT CLOSED?");
            SetText("Client disconnected");
            NextClient();
        }


        delegate void SetTextCallback(string text);

        private void SetText(string text)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
            if (this.listBox1.InvokeRequired)
            {
                this.BeginInvoke(new SetTextCallback(SetText), new object[] {text});
            }
            else
            {
                this.listBox1.Items.Add(text);
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            try
            {
                server = new TcpListener(System.Net.IPAddress.Any, LOCAL_PORT);
                server.Start();
                SetText("Server listening on port " + LOCAL_PORT);
                NextClient();

                Debug.WriteLine("Form loaded");

                //Create an instance of PowerPoint.
                oPPT = new PowerPoint.Application();

                oPPT.PresentationOpen += new Microsoft.Office.Interop.PowerPoint.EApplication_PresentationOpenEventHandler(oPPT_PresentationOpen);
                oPPT.SlideShowNextClick += new Microsoft.Office.Interop.PowerPoint.EApplication_SlideShowNextClickEventHandler(oPPT_SlideShowNextClick);
                oPPT.SlideShowNextSlide += new Microsoft.Office.Interop.PowerPoint.EApplication_SlideShowNextSlideEventHandler(oPPT_SlideShowNextSlide);
                //oPPT.SlideShowOnNext += new Microsoft.Office.Interop.PowerPoint.EApplication_SlideShowOnNextEventHandler(oPPT_SlideShowOnNext);

                // Show PowerPoint to the user.
                oPPT.Visible = Microsoft.Office.Core.MsoTriState.msoTrue;
            }
            catch (Exception ex) 
            {
                MessageBox.Show("Error: " + ex.Message, "Application error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                this.Close();
                Application.Exit();
            }
        }

        void oPPT_SlideShowNextSlide(Microsoft.Office.Interop.PowerPoint.SlideShowWindow Wn)
        {
            if (SlideEventCB.Checked)
            {
                string name = Wn.View.Slide.Name;


                // WISE Algorithm
                if (TitleSearchCB.Checked)
                {
                    if ((Wn.View.Slide.Shapes.HasTitle == MsoTriState.msoTrue) && (Wn.View.Slide.Shapes.Title.HasTextFrame == MsoTriState.msoTrue))
                    {
                        name = Wn.View.Slide.Shapes.Title.TextFrame.TextRange.Text;
                    }
                    else
                    {
                        float top = 10000;

                        foreach (PowerPoint.Shape shape in Wn.View.Slide.Shapes)
                        {
                            if ((shape.HasTextFrame == MsoTriState.msoTrue) && (top > shape.TextFrame.TextRange.BoundTop))
                            {
                                name = shape.TextFrame.TextRange.Text;
                                top = shape.TextFrame.TextRange.BoundTop;
                            }
                        }
                    }
                }
                string text = "NextSlide: " + name;
                text = text.Replace('\n', ' ').Replace('\v', ' ');
                SetText(text);
                WriteToSocket(text);
            }
        }

        void oPPT_SlideShowOnNext(Microsoft.Office.Interop.PowerPoint.SlideShowWindow Wn)
        {
            SetText("OnNext");
        }

        void oPPT_PresentationOpen(Microsoft.Office.Interop.PowerPoint.Presentation Pres)
        {
            if (FileEventCB.Checked)
            {
                string text = "Presentation Open: " + Pres.Name;
                SetText(text);
                WriteToSocket(text);
            }
        }

        void oPPT_SlideShowNextClick(Microsoft.Office.Interop.PowerPoint.SlideShowWindow Wn, Microsoft.Office.Interop.PowerPoint.Effect nEffect)
        {
            if (NextClickCB.Checked)
            {
                SetText("NextClick");
                WriteToSocket("NextClick");
            }
        }

        private void WriteToSocket(string text)
        {
            if (stream != null)
            {
                try
                {
                    stream.WriteLine(text);
                    stream.Flush();
                }
                catch (IOException ioe)
                {
                    Debug.WriteLine("ERROR AL ESCRIBIR: " + ioe.Message);
                    NextClient();
                }
            }
        }


        private void Start_Click(object sender, EventArgs e)
        {
            PowerPoint.Presentations oPresSet = oPPT.Presentations;
            PowerPoint._Presentation oPres = oPresSet.Open("c:\\pres1.ppt",MsoTriState.msoFalse, MsoTriState.msoFalse, MsoTriState.msoTrue);
            //oPres.SlideShowSettings.Run();
        }

        private void ClearButton_Click(object sender, EventArgs e)
        {
            listBox1.Items.Clear();
        }

        private void LaunchButton_Click(object sender, EventArgs e)
        {
            // Show PowerPoint to the user.
            oPPT.Visible = Microsoft.Office.Core.MsoTriState.msoTrue;
        }

        private void Form1_Resize(object sender, EventArgs e)
        {
            if (FormWindowState.Minimized == WindowState)
            {
                Hide();
                notifyIcon1.Visible = true;
            }
        }

        private void notifyIcon1_DoubleClick(object sender, EventArgs e)
        {
            Show();
            WindowState = FormWindowState.Normal;
            notifyIcon1.Visible = false;
        }
    }
}