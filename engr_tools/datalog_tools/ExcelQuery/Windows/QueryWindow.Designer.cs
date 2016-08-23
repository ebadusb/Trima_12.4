namespace ExcelQuery.Windows {
	partial class QueryWindow {
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
			this.mQueryTextbox = new System.Windows.Forms.TextBox();
			this.mSecondButton = new System.Windows.Forms.Button();
			this.mFirstButton = new System.Windows.Forms.Button();
			this.mQueryList = new System.Windows.Forms.ComboBox();
			this.mDestinationLabel = new System.Windows.Forms.Label();
			this.mDestinationTextfield = new System.Windows.Forms.TextBox();
			this.SuspendLayout();
			// 
			// mQueryTextbox
			// 
			this.mQueryTextbox.AcceptsReturn = true;
			this.mQueryTextbox.AcceptsTab = true;
			this.mQueryTextbox.Font = new System.Drawing.Font("Courier New", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.mQueryTextbox.Location = new System.Drawing.Point(12, 40);
			this.mQueryTextbox.Multiline = true;
			this.mQueryTextbox.Name = "mQueryTextbox";
			this.mQueryTextbox.ScrollBars = System.Windows.Forms.ScrollBars.Both;
			this.mQueryTextbox.Size = new System.Drawing.Size(470, 150);
			this.mQueryTextbox.TabIndex = 0;
			this.mQueryTextbox.WordWrap = false;
			// 
			// mSecondButton
			// 
			this.mSecondButton.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.mSecondButton.Location = new System.Drawing.Point(407, 196);
			this.mSecondButton.Name = "mSecondButton";
			this.mSecondButton.Size = new System.Drawing.Size(75, 23);
			this.mSecondButton.TabIndex = 2;
			this.mSecondButton.Text = "Run";
			this.mSecondButton.UseVisualStyleBackColor = true;
			// 
			// mFirstButton
			// 
			this.mFirstButton.Location = new System.Drawing.Point(326, 196);
			this.mFirstButton.Name = "mFirstButton";
			this.mFirstButton.Size = new System.Drawing.Size(75, 23);
			this.mFirstButton.TabIndex = 3;
			this.mFirstButton.Text = "Save";
			this.mFirstButton.UseVisualStyleBackColor = true;
			// 
			// mQueryList
			// 
			this.mQueryList.FormattingEnabled = true;
			this.mQueryList.Location = new System.Drawing.Point(12, 13);
			this.mQueryList.Name = "mQueryList";
			this.mQueryList.Size = new System.Drawing.Size(470, 21);
			this.mQueryList.TabIndex = 4;
			this.mQueryList.SelectedIndexChanged += new System.EventHandler(this.QueryListSelectionHandler);
			// 
			// mDestinationLabel
			// 
			this.mDestinationLabel.AutoSize = true;
			this.mDestinationLabel.Location = new System.Drawing.Point(12, 201);
			this.mDestinationLabel.Name = "mDestinationLabel";
			this.mDestinationLabel.Size = new System.Drawing.Size(63, 13);
			this.mDestinationLabel.TabIndex = 5;
			this.mDestinationLabel.Text = "Destination:";
			// 
			// mDestinationTextfield
			// 
			this.mDestinationTextfield.Location = new System.Drawing.Point(81, 198);
			this.mDestinationTextfield.Name = "mDestinationTextfield";
			this.mDestinationTextfield.Size = new System.Drawing.Size(150, 20);
			this.mDestinationTextfield.TabIndex = 6;
			this.mDestinationTextfield.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.KeyPressHandler);
			// 
			// QueryWindow
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(494, 233);
			this.Controls.Add(this.mDestinationTextfield);
			this.Controls.Add(this.mDestinationLabel);
			this.Controls.Add(this.mQueryList);
			this.Controls.Add(this.mFirstButton);
			this.Controls.Add(this.mSecondButton);
			this.Controls.Add(this.mQueryTextbox);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
			this.Name = "QueryWindow";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "Query";
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		protected System.Windows.Forms.Button mSecondButton;
		protected System.Windows.Forms.Button mFirstButton;
		protected System.Windows.Forms.TextBox mQueryTextbox;
		protected System.Windows.Forms.ComboBox mQueryList;
		protected System.Windows.Forms.Label mDestinationLabel;
		protected System.Windows.Forms.TextBox mDestinationTextfield;
	}
}