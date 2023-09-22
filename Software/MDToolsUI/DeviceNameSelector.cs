using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Terminal.Gui;

namespace MDToolsUI
{
    public class DeviceNameSelector : Dialog
    {
        CheckBox ckFlp;
        CheckBox ckFdk;
        CheckBox ckDsk;
        CheckBox ckDrv;
        public string[]? SelectedNames { get; set; }
        public DeviceNameSelector()
        { 
            Title = "Select device names";

            Width = 25;
            Height = 9;

            ckFlp = new CheckBox(9, 1, "flp") { Checked = true };
            ckFdk = new CheckBox(9, 2, "fdk") { Checked = true };
            ckDsk = new CheckBox(9, 3, "dsk") { Checked = true };
            ckDrv = new CheckBox(9, 4, "drv") { Checked = true };

            Add(ckFlp);
            Add(ckFdk);
            Add(ckDsk);
            Add(ckDrv);
        }

        public override void OnLoaded()
        {
            base.OnLoaded();

            Button ok = new Button("Ok");
            ok.Clicked += () =>
            {

                if(!ckFlp.Checked && !ckFdk.Checked && !ckDsk.Checked && !ckDrv.Checked)
                {
                    MessageBox.ErrorQuery(Title, "At least one device name must be selected.", "Ok");
                    return;
                }

                List<string> selectedNames = new List<string>();
                if (ckFlp.Checked) selectedNames.Add("flp");
                if (ckFdk.Checked) selectedNames.Add("fdk");
                if (ckDsk.Checked) selectedNames.Add("dsk");
                if (ckDrv.Checked) selectedNames.Add("drv");
                SelectedNames = selectedNames.ToArray();

                Application.RequestStop();
            };

            AddButton(ok);

            Button cancel = new Button("Cancel");
            cancel.Clicked += () =>
            {
                SelectedNames = null;
                Application.RequestStop();
            };

            AddButton(cancel);
        }
    }
}
