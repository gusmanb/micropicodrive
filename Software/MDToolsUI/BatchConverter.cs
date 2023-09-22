using MicroDriveTools.Classes;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Terminal.Gui;

namespace MDToolsUI
{
    public class BatchConverter : Dialog
    {
        CheckBox ckFix;
        string[]? selectedNames;
        Button? btnConvert;
        Button? btnCancel;

        public BatchConverter()
        {
            Title = "Batch converter";
            Width = 30;
            Height = 10;
            ckFix = new CheckBox("Fix drives") { Checked = true, X = Pos.Center(), Y = 1  };

            Add(ckFix);

            Button btnSelectFiles = new Button("Select files") { X = Pos.Center(), Y = 3 };
            btnSelectFiles.X = Pos.Center();

            btnSelectFiles.Clicked += () =>
            {
                var dlg = new OpenDialog("Select files", "Select files to convert", new List<string>() { ".zip", ".mdv" }, OpenDialog.OpenMode.File) { AllowsMultipleSelection = true } ;

                Application.Run(dlg);

                if (!dlg.Canceled && dlg.FilePaths != null && dlg.FilePaths.Count > 0)
                    selectedNames = dlg.FilePaths.ToArray();
                else
                    selectedNames = null;

                if(btnConvert != null)
                    btnConvert.Enabled = selectedNames != null && selectedNames.Length > 0;
            };

            Add(btnSelectFiles);
        }

        public override void OnLoaded()
        {
            base.OnLoaded();
            btnConvert = new Button("Convert");
            btnConvert.Clicked += () =>
            {
                if (selectedNames == null || selectedNames.Length == 0)
                    return;

                var dlg = new OpenDialog("Destination", "Select destination folder", null, OpenDialog.OpenMode.Directory);

                Application.Run(dlg);

                if (dlg.Canceled || dlg.FilePath == null)
                    return;

                var dest = dlg.FilePath;

                string[]? fixNames = null;

                if (ckFix.Checked)
                {
                    var nameSelector = new DeviceNameSelector();
                    Application.Run(nameSelector);

                    if (nameSelector.SelectedNames != null && nameSelector.SelectedNames.Length > 0)
                        fixNames = nameSelector.SelectedNames;
                    else
                        return;
                }

                foreach(var file in selectedNames)
                {
                    try
                    {
                        MicroDriveDirectory? dir = null;

                        if (file.EndsWith(".zip"))
                            dir = ZIPImporter.ImportZIPFile(file);
                        else if (file.EndsWith(".mdv"))
                        {
                            var mdv = MicroDriveCartridge.LoadMDV(file);
                            dir = mdv.Directory;
                        }

                        string mediumName = System.IO.Path.GetFileNameWithoutExtension(file).Replace(".", "_");

                        if (mediumName.Length > 10)
                            mediumName = mediumName.Substring(0, 10);

                        if (fixNames != null)
                        {
                            foreach (var bFile in dir.Files)
                            {
                                byte[] data = bFile.Data;
                                Utils.ReplaceDriveNames(data, fixNames);
                                bFile.UpdateData(data);

                            }
                        }

                        MicroDriveCartridge cartridge = new MicroDriveCartridge(dir, mediumName);
                        var newFile = System.IO.Path.Combine(Path.GetDirectoryName(file), System.IO.Path.GetFileNameWithoutExtension(file) + ".mdv");

                        cartridge.SaveMDV(newFile);
                    }
                    catch(Exception ex) { MessageBox.ErrorQuery("Error", $"Error converting file \"{Path.GetFileName(file)}\": {ex.Message}.", "Ok"); return; }
                }

                MessageBox.Query("Done", "Conversion completed.", "Ok");

                Application.RequestStop();
            };
            btnConvert.Enabled = false;

            btnCancel = new Button("Cancel");
            btnCancel.Clicked += () =>
            {
                Application.RequestStop();
            };

            AddButton(btnConvert);
            AddButton(btnCancel);

        }
    }
}
