using MicroDriveTools.Classes;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Linq;
using System.Management;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using Terminal.Gui;

namespace MDToolsUI
{
    public class DirectoryExplorer : Window
    {
        TableView directoryTable;
        DirectoryTableSource source;
        MicroDriveDirectory directory;

        public DirectoryExplorer()
        {

            Title = "MicroDrive Tools V1.0 - El Dr. Gusman";

            MenuBar menu = new MenuBar(new MenuBarItem[]
            {
                new MenuBarItem ("_File", new MenuItem []
                {
                    new MenuItem ("_Import ZIP/MDV", "", () => { BtnImport_Clicked(); }),
                    null,
                    new MenuItem ("_Add file", "", () => { BtnAdd_Clicked(); }),
                    new MenuItem ("_Remove file", "", () => { BtnRemove_Clicked(); }),
                    new MenuItem ("_Rename file", "", () => { BtnRename_Clicked(); }),
                    new MenuItem ("Set E_xecutable", "", () => { BtnExec_Clicked(); }),
                    null,
                    new MenuItem ("_Export file", "", () => { BtnExport_Clicked(); }),
                    new MenuItem ("_Save cartridge", "", () => { BtnSave_Clicked(); }),
                    null,
                    new MenuItem ("_Quit", "", () => { Application.RequestStop(); })
                }),
                new MenuBarItem("_Tools", new MenuItem[]
                {
                    new MenuItem("_Fix drive name", "", () => { FixDrives_Clicked(); }),
                    new MenuItem("_Batch convert", "", () => { BatchConvert_Clicked(); })
                }),
                new MenuBarItem ("_Help", new MenuItem []
                {
                    new MenuItem ("_About", "", () => { MessageBox.Query("About", "MicroDrive Tools V1.0\n\nDeveloped by El Dr. Gusman\n\nhttps://github.com/gusmanb/micropicodrive", "Ok"); })
                })
            });

            menu.ColorScheme = Colors.Dialog;

            Add(menu);

            Label lbl = new Label("- Microdrive directory -")
            {
                X = 0,
                Y = 2,
                Width = Dim.Fill(),
                Height = 1,
                TextAlignment = TextAlignment.Centered
            };

            var tblScheme = Colors.Menu;
            tblScheme.Normal = Application.Driver.MakeAttribute(Color.Black, Color.BrightCyan);

            //Add(lbl);

            directory = new MicroDriveDirectory();

            source = new DirectoryTableSource(directory);

            directoryTable = new TableView()
            {
                X = 1,
                Y = 2,
                Width = Dim.Fill(1),
                Height = Dim.Fill(2),
                FullRowSelect = true,
                MultiSelect = false,
                ColorScheme = tblScheme
            };

            var style = new TableView.TableStyle();
            style.AlwaysShowHeaders = true;
            style.ShowVerticalCellLines = false;
            style.ExpandLastColumn = false;

            style.ColumnStyles = new Dictionary<DataColumn, TableView.ColumnStyle>()
            {
               { source.Columns[0],  new TableView.ColumnStyle() { MinWidth = 36 } },
               { source.Columns[1],  new TableView.ColumnStyle() { MinWidth = 8, Alignment = TextAlignment.Right } },
               { source.Columns[2],  new TableView.ColumnStyle() { MinWidth = 0, Alignment = TextAlignment.Centered } },
               { source.Columns[3],  new TableView.ColumnStyle() { MinWidth = 6, Alignment = TextAlignment.Right } },
            };

            directoryTable.Style = style;
            directoryTable.Table = source;

            Add(directoryTable);

            SetupScrollBar();

            Button btnImport = new Button("Import".PadRight(6))
            {
                Y = Pos.AnchorEnd(1)
            };

            Button btnAdd = new Button("Add".PadRight(6))
            {
                Y = Pos.AnchorEnd(1)
            };

            Button btnRemove = new Button("Remove".PadRight(6))
            {
                Y = Pos.AnchorEnd(1)
            };

            Button btnRename = new Button("Re_name".PadRight(6))
            {
                Y = Pos.AnchorEnd(1)
            };

            Button btnExec = new Button("Exec".PadRight(6))
            {
                Y = Pos.AnchorEnd(1)
            };

            Button btnExport = new Button("Export".PadRight(6))
            {
                Y = Pos.AnchorEnd(1)
            };

            Button btnSave = new Button("Save".PadRight(6))
            {
                Y = Pos.AnchorEnd(1)
            };

            Button btnQuit = new Button("Quit".PadRight(6))
            {
                Y = Pos.AnchorEnd(1),
                X = 0
            };

            btnImport.X = Pos.AnchorEnd(12) - 60;
            btnAdd.X = Pos.AnchorEnd(12) - 50;
            btnRemove.X = Pos.AnchorEnd(12) - 40;
            btnRename.X = Pos.AnchorEnd(12) - 30;
            btnExec.X = Pos.AnchorEnd(12) - 20;
            btnExport.X = Pos.AnchorEnd(12) - 10;
            btnSave.X = Pos.AnchorEnd(12);

            btnImport.Clicked += BtnImport_Clicked;
            btnAdd.Clicked += BtnAdd_Clicked;
            btnRemove.Clicked += BtnRemove_Clicked;
            btnRename.Clicked += BtnRename_Clicked;
            btnExec.Clicked += BtnExec_Clicked;
            btnExport.Clicked += BtnExport_Clicked;
            btnSave.Clicked += BtnSave_Clicked;
            btnQuit.Clicked += () => Application.RequestStop();

            Add(btnImport, btnAdd, btnRemove, btnRename, btnExec, btnExport, btnSave, btnQuit);
            LayoutSubviews();
        }

        private void BatchConvert_Clicked()
        {
            var dlg = new BatchConverter();
            Application.Run(dlg);
        }

        private void FixDrives_Clicked()
        {
            var dlg = new DeviceNameSelector();
            Application.Run(dlg);
            
            if(dlg.SelectedNames == null)
                return;

            int matches = 0;

            foreach (var file in directory.Files)
            {
                byte[] data = file.Data;
                int occurrences = Utils.ReplaceDriveNames(data, dlg.SelectedNames);

                if (occurrences > 0)
                {
                    file.UpdateData(data);
                    matches += occurrences;
                }
            }

            if(matches == 0)
                MessageBox.Query("Fix drive name", "No matches found.", "Ok");
            else
                MessageBox.Query("Fix drive name", $"Drive names fixed: {matches}", "Ok");
        }

        

        private void BtnExport_Clicked()
        {
            try
            {
                if (directoryTable.SelectedRow != -1)
                {
                    var row = directoryTable.Table.Rows[directoryTable.SelectedRow];

                    var file = directory.Files.FirstOrDefault(f => f.Header.FileName == row[0].ToString());

                    if (file == null)
                        return;

                    SaveDialog dlg = new SaveDialog($"Export file \"{file.Header.FileName}\"", "Select the path where to export the file");

                    Application.Run(dlg);

                    if (dlg.Canceled || dlg.FileName == null)
                        return;

                    File.WriteAllBytes(dlg.FilePath.ToString(), file.Data);
                }
            }
            catch (Exception ex)
            {
                MessageBox.ErrorQuery("Error", ex.Message, "Ok");
            }
        }

        private void BtnSave_Clicked()
        {
            try
            {
                if (directory.Files.Length == 0)
                {
                    MessageBox.ErrorQuery("Error", "Directory is empty.", "Ok");
                    return;
                }

                var saveDialog = new SaveDialog("Save cartridge", "Select the path where to store the cartridge", new List<string>() { ".mdv" });

                Application.Run(saveDialog);

                if (saveDialog.Canceled || saveDialog.FileName == null)
                    return;

                string fName = saveDialog.FileName.ToString() ?? "";

                /*
                var mediumDlg = new InputBox("Medium name", "Enter the name of the medium", Path.GetFileNameWithoutExtension(fName).Replace(".", "_"));

                Application.Run(mediumDlg);

                if (mediumDlg.Input == null)
                    return;
                */

                var mediumDlg = new MediaInfo(Path.GetFileNameWithoutExtension(fName).Replace(".", "_"));

                Application.Run(mediumDlg);

                if (mediumDlg.MediaName == null)
                    return;

                MicroDriveCartridge cartridge = new MicroDriveCartridge(directory, mediumDlg.MediaName, MediaIdentifier: mediumDlg.MediaId);
                cartridge.SaveMDV(saveDialog.FilePath.ToString());

                MessageBox.Query("Success", "Cartridge saved successfully.", "Ok");
            }
            catch (Exception ex)
            {
                MessageBox.ErrorQuery("Error", ex.Message, "Ok");
            }
        }

        private void BtnExec_Clicked()
        {
            try
            {
                if (directoryTable.SelectedRow != -1)
                {
                    var row = directoryTable.Table.Rows[directoryTable.SelectedRow];

                    var file = directory.Files.FirstOrDefault(f => f.Header.FileName == row[0].ToString());

                    if (file == null)
                        return;

                    if (file.Header.FileType == 1)
                    {
                        file.UpdateExecutable(false);
                    }
                    else
                    {
                        var ib = new InputBox("Data space", "Enter the data space for the file", file.Header.DataSpace.ToString());

                        Application.Run(ib);

                        if (ib.Input == null)
                            return;

                        if (!uint.TryParse(ib.Input, out _))
                        {
                            MessageBox.ErrorQuery("Error", "Invalid data space.", "Ok");
                            return;
                        }

                        file.UpdateExecutable(true, uint.Parse(ib.Input));
                    }

                    source.Refresh();
                    directoryTable.SetNeedsDisplay();
                }
            }
            catch (Exception ex)
            {
                MessageBox.ErrorQuery("Error", ex.Message, "Ok");
            }
        }

        private void BtnRename_Clicked()
        {
            try
            {
                if (directoryTable.SelectedRow != -1)
                {
                    var row = directoryTable.Table.Rows[directoryTable.SelectedRow];

                    var file = directory.Files.FirstOrDefault(f => f.Header.FileName == row[0].ToString());

                    if (file == null)
                        return;

                    var ib = new InputBox("New name", "Enter the new name for the file", row[0].ToString());

                    Application.Run(ib);

                    if (ib.Input == null)
                        return;

                    file.UpdateFileName(ib.Input);
                    source.Refresh();
                    directoryTable.SetNeedsDisplay();
                }
            }
            catch (Exception ex)
            {
                MessageBox.ErrorQuery("Error", ex.Message, "Ok");
            }
        }

        private void BtnRemove_Clicked()
        {
            try
            {
                if (directoryTable.SelectedRow != -1)
                {
                    var row = directoryTable.Table.Rows[directoryTable.SelectedRow];
                    var result = MessageBox.Query("Remove", $"Do you want to remove file \"{row[0]}\"?", "Yes", "No");

                    if (result == 1)
                        return;

                    directory.RemoveFile(row[0].ToString());
                    source.Refresh();
                    directoryTable.SetNeedsDisplay();
                }
            }
            catch (Exception ex)
            {
                MessageBox.ErrorQuery("Error", ex.Message, "Ok");
            }
        }

        private void BtnAdd_Clicked()
        {
            OpenDialog dlg = new OpenDialog("Add", "Select files to add.")
            {
                AllowsMultipleSelection = true
            };

            Application.Run(dlg);

            if (dlg.Canceled || dlg.FilePaths == null || dlg.FilePaths.Count == 0)
                return;

            var file = dlg.FilePaths[0];

            try
            {
                MicroDriveFile fileToAdd = new MicroDriveFile(Path.GetFileNameWithoutExtension(file).Replace(".", "_"), File.ReadAllBytes(file));

                directory.AddFile(fileToAdd);
                source.Refresh();
                directoryTable.SetNeedsDisplay();
            }
            catch (Exception ex)
            {
                MessageBox.ErrorQuery("Error", ex.Message, "Ok");
            }
        }

        private void BtnImport_Clicked()
        {
            OpenDialog dlg = new OpenDialog("Import", "Select import source.", new List<string>() { ".mdv", ".zip" })
            {
                AllowsMultipleSelection = false
            };

            Application.Run(dlg);

            if(dlg.Canceled || dlg.FilePaths == null || dlg.FilePaths.Count == 0)
                return;
            
            var file = dlg.FilePaths[0];

            int result = 0;

            if (source.Directory.Files.Length > 0)
            {
                result = MessageBox.Query("Import", "Do you want to append to the current directory or replace it?", "Append", "Replace", "Cancel");

                if (result == 2)
                    return;
            }

            try
            {
                if (Path.GetExtension(file).ToLower() == ".mdv")
                    ImportMDV(file, result == 0);
                else
                    ImportZIP(file, result == 0);

            }
            catch (Exception ex)
            {
                MessageBox.ErrorQuery("Error", ex.Message + " - " + ex.StackTrace, "Ok");
            }
        }

        private void ImportZIP(string fileName, bool append)
        {
            var dir = ZIPImporter.ImportZIPFile(fileName);

            if (append)
            {
                foreach (var file in dir.Files)
                    directory.AddFile(file);
            }
            else
                source.Directory = directory = dir;

            source.Refresh();
            directoryTable.SetNeedsDisplay();
        }

        private void ImportMDV(string fileName, bool append)
        {
            var cart = MicroDriveCartridge.LoadMDV(fileName);

            if (append)
            {
                foreach (var file in cart.Directory.Files)
                    directory.AddFile(file);
            }
            else
                source.Directory = directory = cart.Directory;

            source.Refresh();
            directoryTable.SetNeedsDisplay();
        }

        private void SetupScrollBar()
        {
            var _scrollBar = new ScrollBarView(directoryTable, true);

            _scrollBar.ChangedPosition += () => {
                directoryTable.RowOffset = _scrollBar.Position;
                if (directoryTable.RowOffset != _scrollBar.Position)
                {
                    _scrollBar.Position = directoryTable.RowOffset;
                }
                directoryTable.SetNeedsDisplay();
            };

            directoryTable.DrawContent += (e) => {
                _scrollBar.Size = directoryTable.Table?.Rows?.Count ?? 0;
                _scrollBar.Position = directoryTable.RowOffset;
                _scrollBar.Refresh();
            };

        }

        class DirectoryTableSource : DataTable
        {
            static string[] columnNames = new string[] { "Name", "Size", "Executable", "Data space" };
            MicroDriveDirectory directory;

            public MicroDriveDirectory Directory { get => directory; set => directory = value; }

            public DirectoryTableSource(MicroDriveDirectory Directory)
            {
                directory = Directory;

                this.Columns.Add("Name");
                this.Columns.Add("Size");
                this.Columns.Add("Executable");
                this.Columns.Add("Data space");

                Refresh();
            }

            public void Refresh()
            {
                this.Rows.Clear();

                foreach (var file in directory.Files)
                    this.Rows.Add(new object[] { file.Header.FileName, file.Data.Length, file.Header.FileType == 1 ? "[X]" : "[ ]", file.Header.DataSpace });
            }
        }
    }
}
