namespace PowerPointServer
{
    partial class Form1
    {
        /// <summary>
        /// Variable del diseñador requerida.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Limpiar los recursos que se estén utilizando.
        /// </summary>
        /// <param name="disposing">true si los recursos administrados se deben eliminar; false en caso contrario, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);

            //oPPT.Quit();
        }

        #region Código generado por el Diseñador de Windows Forms

        /// <summary>
        /// Método necesario para admitir el Diseñador. No se puede modificar
        /// el contenido del método con el editor de código.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.listBox1 = new System.Windows.Forms.ListBox();
            this.TitleSearchCB = new System.Windows.Forms.CheckBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.NextClickCB = new System.Windows.Forms.CheckBox();
            this.FileEventCB = new System.Windows.Forms.CheckBox();
            this.SlideEventCB = new System.Windows.Forms.CheckBox();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.flowLayoutPanel1 = new System.Windows.Forms.FlowLayoutPanel();
            this.LaunchButton = new System.Windows.Forms.Button();
            this.ClearButton = new System.Windows.Forms.Button();
            this.toolTip1 = new System.Windows.Forms.ToolTip(this.components);
            this.notifyIcon1 = new System.Windows.Forms.NotifyIcon(this.components);
            this.groupBox1.SuspendLayout();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.flowLayoutPanel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // listBox1
            // 
            this.listBox1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listBox1.FormattingEnabled = true;
            this.listBox1.Location = new System.Drawing.Point(0, 0);
            this.listBox1.Name = "listBox1";
            this.listBox1.Size = new System.Drawing.Size(210, 511);
            this.listBox1.TabIndex = 1;
            // 
            // TitleSearchCB
            // 
            this.TitleSearchCB.AutoSize = true;
            this.TitleSearchCB.Checked = true;
            this.TitleSearchCB.CheckState = System.Windows.Forms.CheckState.Checked;
            this.TitleSearchCB.Location = new System.Drawing.Point(6, 19);
            this.TitleSearchCB.Name = "TitleSearchCB";
            this.TitleSearchCB.Size = new System.Drawing.Size(135, 17);
            this.TitleSearchCB.TabIndex = 3;
            this.TitleSearchCB.Text = "WISE® Title Discovery";
            this.TitleSearchCB.UseVisualStyleBackColor = true;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.NextClickCB);
            this.groupBox1.Controls.Add(this.FileEventCB);
            this.groupBox1.Controls.Add(this.SlideEventCB);
            this.groupBox1.Controls.Add(this.TitleSearchCB);
            this.groupBox1.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBox1.Location = new System.Drawing.Point(0, 0);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(201, 126);
            this.groupBox1.TabIndex = 4;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Configuration";
            // 
            // NextClickCB
            // 
            this.NextClickCB.AutoSize = true;
            this.NextClickCB.Location = new System.Drawing.Point(4, 90);
            this.NextClickCB.Name = "NextClickCB";
            this.NextClickCB.Size = new System.Drawing.Size(105, 17);
            this.NextClickCB.TabIndex = 6;
            this.NextClickCB.Text = "Next Click Event";
            this.NextClickCB.UseVisualStyleBackColor = true;
            // 
            // FileEventCB
            // 
            this.FileEventCB.AutoSize = true;
            this.FileEventCB.Location = new System.Drawing.Point(6, 66);
            this.FileEventCB.Name = "FileEventCB";
            this.FileEventCB.Size = new System.Drawing.Size(102, 17);
            this.FileEventCB.TabIndex = 5;
            this.FileEventCB.Text = "Open File Event";
            this.FileEventCB.UseVisualStyleBackColor = true;
            // 
            // SlideEventCB
            // 
            this.SlideEventCB.AutoSize = true;
            this.SlideEventCB.Checked = true;
            this.SlideEventCB.CheckState = System.Windows.Forms.CheckState.Checked;
            this.SlideEventCB.Location = new System.Drawing.Point(6, 42);
            this.SlideEventCB.Name = "SlideEventCB";
            this.SlideEventCB.Size = new System.Drawing.Size(105, 17);
            this.SlideEventCB.TabIndex = 4;
            this.SlideEventCB.Text = "Next Slide Event";
            this.SlideEventCB.UseVisualStyleBackColor = true;
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.flowLayoutPanel1);
            this.splitContainer1.Panel1.Controls.Add(this.groupBox1);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.listBox1);
            this.splitContainer1.Size = new System.Drawing.Size(415, 517);
            this.splitContainer1.SplitterDistance = 201;
            this.splitContainer1.TabIndex = 5;
            // 
            // flowLayoutPanel1
            // 
            this.flowLayoutPanel1.Controls.Add(this.LaunchButton);
            this.flowLayoutPanel1.Controls.Add(this.ClearButton);
            this.flowLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.flowLayoutPanel1.Location = new System.Drawing.Point(0, 478);
            this.flowLayoutPanel1.Name = "flowLayoutPanel1";
            this.flowLayoutPanel1.Padding = new System.Windows.Forms.Padding(5);
            this.flowLayoutPanel1.Size = new System.Drawing.Size(201, 39);
            this.flowLayoutPanel1.TabIndex = 7;
            // 
            // LaunchButton
            // 
            this.LaunchButton.Location = new System.Drawing.Point(8, 8);
            this.LaunchButton.Name = "LaunchButton";
            this.LaunchButton.Size = new System.Drawing.Size(75, 23);
            this.LaunchButton.TabIndex = 6;
            this.LaunchButton.Text = "Launch";
            this.toolTip1.SetToolTip(this.LaunchButton, "Launch PowerPoint");
            this.LaunchButton.UseVisualStyleBackColor = true;
            this.LaunchButton.Click += new System.EventHandler(this.LaunchButton_Click);
            // 
            // ClearButton
            // 
            this.ClearButton.Location = new System.Drawing.Point(89, 8);
            this.ClearButton.Name = "ClearButton";
            this.ClearButton.Size = new System.Drawing.Size(75, 23);
            this.ClearButton.TabIndex = 5;
            this.ClearButton.Text = "Clear";
            this.toolTip1.SetToolTip(this.ClearButton, "Clear Event List");
            this.ClearButton.UseVisualStyleBackColor = true;
            this.ClearButton.Click += new System.EventHandler(this.ClearButton_Click);
            // 
            // notifyIcon1
            // 
            this.notifyIcon1.Icon = ((System.Drawing.Icon)(resources.GetObject("notifyIcon1.Icon")));
            this.notifyIcon1.Text = "PowerPoint Isabel Server";
            this.notifyIcon1.DoubleClick += new System.EventHandler(this.notifyIcon1_DoubleClick);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size(415, 517);
            this.Controls.Add(this.splitContainer1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MaximumSize = new System.Drawing.Size(423, 544);
            this.MinimumSize = new System.Drawing.Size(423, 544);
            this.Name = "Form1";
            this.Text = "PowerPoint Isabel Server";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.Resize += new System.EventHandler(this.Form1_Resize);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.ResumeLayout(false);
            this.flowLayoutPanel1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ListBox listBox1;
        private System.Windows.Forms.CheckBox TitleSearchCB;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.CheckBox FileEventCB;
        private System.Windows.Forms.CheckBox SlideEventCB;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.CheckBox NextClickCB;
        private System.Windows.Forms.Button LaunchButton;
        private System.Windows.Forms.Button ClearButton;
        private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel1;
        private System.Windows.Forms.ToolTip toolTip1;
        private System.Windows.Forms.NotifyIcon notifyIcon1;
    }
}

