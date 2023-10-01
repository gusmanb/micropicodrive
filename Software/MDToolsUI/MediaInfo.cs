using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Terminal.Gui;

namespace MDToolsUI
{
    public class MediaInfo : Dialog
    {

        string? selectedName;
        ushort? selectedId;

        public string? MediaName { get { return selectedName; } }
        public ushort? MediaId { get { return selectedId; } }

        TextField mediaName;
        CheckBox ckSpecify;
        TextField mediaId;

        public MediaInfo(string DefaultName)
        {
            Title = "Media Info";
            Width = Dim.Percent(50);
            Height = 12;
            
            Label prompt = new Label(2, 1, "Media name");

            Add(prompt);

            mediaName = new TextField(DefaultName);
            mediaName.X = 2;
            mediaName.Y = 3;
            mediaName.Width = Dim.Fill(2);

            Add(mediaName);

            ckSpecify = new CheckBox(2, 5, "Specify media ID");
            ckSpecify.Toggled += (e) =>
            {
                mediaId.Enabled = ckSpecify.Checked;
            };

            Add(ckSpecify);

            mediaId = new TextField("0x0000");
            mediaId.X = 2;
            mediaId.Y = 7;
            mediaId.Width = Dim.Fill(2);
            mediaId.Enabled = false;
            Add(mediaId);

        }

        public override void OnLoaded()
        {
            base.OnLoaded();

            Button ok = new Button("Ok");
            ok.Clicked += () =>
            {

                if (string.IsNullOrWhiteSpace(mediaName.Text.ToString()))
                {
                    MessageBox.ErrorQuery(Title, "Media name cannot be empty.", "Ok");
                    return;
                }

                ushort id = 0;

                if (ckSpecify.Checked)// && !ushort.TryParse(mediaId.Text.ToString(), System.Globalization.NumberStyles.HexNumber, null, out id))
                {
                    string? strId = mediaId.Text.ToString()?.Trim();

                    if (strId == null || strId.Length == 0)
                    {
                        MessageBox.ErrorQuery(Title, "Invalid media ID.", "Ok");
                        return;
                    }

                    if (strId.StartsWith("0x"))
                        strId = strId.Substring(2);

                    if (!ushort.TryParse(strId, System.Globalization.NumberStyles.HexNumber, null, out id))
                    {
                        MessageBox.ErrorQuery(Title, "Invalid media ID.", "Ok");
                        return;
                    }

                    selectedId = id;
                }
                else
                    selectedId = null;

                selectedName = mediaName.Text.ToString();
                Application.RequestStop();
            };

            AddButton(ok);

            Button cancel = new Button("Cancel");
            cancel.Clicked += () =>
            {
                selectedName = null;
                selectedId = null;
                Application.RequestStop();
            };

            AddButton(cancel);

            mediaName.SetFocus();
        }
    }
}
