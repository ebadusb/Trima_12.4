namespace DatalogLauncher {
	partial class LauncherWindowSimple {
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(LauncherWindowSimple));
			this.mLogTextfield = new System.Windows.Forms.TextBox();
			this.mOKButton = new System.Windows.Forms.Button();
			this.mCancelButton = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// mLogTextfield
			// 
			this.mLogTextfield.AllowDrop = true;
			this.mLogTextfield.Location = new System.Drawing.Point(12, 10);
			this.mLogTextfield.Name = "mLogTextfield";
			this.mLogTextfield.Size = new System.Drawing.Size(179, 20);
			this.mLogTextfield.TabIndex = 0;
			// 
			// mOKButton
			// 
			this.mOKButton.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.mOKButton.Location = new System.Drawing.Point(197, 8);
			this.mOKButton.Name = "mOKButton";
			this.mOKButton.Size = new System.Drawing.Size(50, 23);
			this.mOKButton.TabIndex = 1;
			this.mOKButton.Text = "OK";
			this.mOKButton.UseVisualStyleBackColor = true;
			this.mOKButton.Click += new System.EventHandler(this.HandleLaunchEvent);
			// 
			// mCancelButton
			// 
			this.mCancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.mCancelButton.Location = new System.Drawing.Point(253, 8);
			this.mCancelButton.Name = "mCancelButton";
			this.mCancelButton.Size = new System.Drawing.Size(50, 23);
			this.mCancelButton.TabIndex = 2;
			this.mCancelButton.Text = "Cancel";
			this.mCancelButton.UseVisualStyleBackColor = true;
			this.mCancelButton.Click += new System.EventHandler(this.HandleCancelEvent);
			// 
			// LauncherWindowSimple
			// 
			this.AcceptButton = this.mOKButton;
			this.AllowDrop = true;
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.CancelButton = this.mCancelButton;
			this.ClientSize = new System.Drawing.Size(311, 40);
			this.Controls.Add(this.mCancelButton);
			this.Controls.Add(this.mOKButton);
			this.Controls.Add(this.mLogTextfield);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "LauncherWindowSimple";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "Datalog Launcher";
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.TextBox mLogTextfield;
		private System.Windows.Forms.Button mOKButton;
		private System.Windows.Forms.Button mCancelButton;
	}
}