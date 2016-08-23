namespace DatalogLauncher {
	partial class LauncherWindowExtended {
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null)) {
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(LauncherWindowExtended));
			this.mCancelButton = new System.Windows.Forms.Button();
			this.mOKButton = new System.Windows.Forms.Button();
			this.mLogTextfield = new System.Windows.Forms.TextBox();
			this.mApplicationList = new System.Windows.Forms.ComboBox();
			this.label1 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.SuspendLayout();
			// 
			// mCancelButton
			// 
			this.mCancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.mCancelButton.Location = new System.Drawing.Point(209, 65);
			this.mCancelButton.Name = "mCancelButton";
			this.mCancelButton.Size = new System.Drawing.Size(50, 23);
			this.mCancelButton.TabIndex = 5;
			this.mCancelButton.Text = "Cancel";
			this.mCancelButton.UseVisualStyleBackColor = true;
			this.mCancelButton.Click += new System.EventHandler(this.HandleCancelEvent);
			// 
			// mOKButton
			// 
			this.mOKButton.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.mOKButton.Location = new System.Drawing.Point(153, 65);
			this.mOKButton.Name = "mOKButton";
			this.mOKButton.Size = new System.Drawing.Size(50, 23);
			this.mOKButton.TabIndex = 4;
			this.mOKButton.Text = "OK";
			this.mOKButton.UseVisualStyleBackColor = true;
			this.mOKButton.Click += new System.EventHandler(this.HandleLaunchEvent);
			// 
			// mLogTextfield
			// 
			this.mLogTextfield.AllowDrop = true;
			this.mLogTextfield.Location = new System.Drawing.Point(80, 39);
			this.mLogTextfield.Name = "mLogTextfield";
			this.mLogTextfield.Size = new System.Drawing.Size(179, 20);
			this.mLogTextfield.TabIndex = 3;
			// 
			// mApplicationList
			// 
			this.mApplicationList.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.mApplicationList.FormattingEnabled = true;
			this.mApplicationList.Location = new System.Drawing.Point(80, 12);
			this.mApplicationList.Name = "mApplicationList";
			this.mApplicationList.Size = new System.Drawing.Size(179, 21);
			this.mApplicationList.TabIndex = 6;
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(12, 15);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(62, 13);
			this.label1.TabIndex = 7;
			this.label1.Text = "Application:";
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(15, 42);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(59, 13);
			this.label2.TabIndex = 8;
			this.label2.Text = "Log Name:";
			// 
			// LauncherWindowExtended
			// 
			this.AcceptButton = this.mOKButton;
			this.AllowDrop = true;
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.CancelButton = this.mCancelButton;
			this.ClientSize = new System.Drawing.Size(271, 100);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.mApplicationList);
			this.Controls.Add(this.mCancelButton);
			this.Controls.Add(this.mOKButton);
			this.Controls.Add(this.mLogTextfield);
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "LauncherWindowExtended";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "Datalog Launcher";
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Button mCancelButton;
		private System.Windows.Forms.Button mOKButton;
		private System.Windows.Forms.TextBox mLogTextfield;
		private System.Windows.Forms.ComboBox mApplicationList;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
	}
}