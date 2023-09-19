using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Terminal.Gui;

namespace MDToolsUI
{
    public class InputBox : Dialog
    {
        TextField input;
        public string? Input { get; set; }
        public InputBox(string Title, string Prompt, string Default = "")
        {

            this.Title = Title;
            Width = Dim.Percent(50);
            Height = 8;
            Label prompt = new Label(1, 1, Prompt);

            Add(prompt);

            input = new TextField(Default);
            input.X = Pos.Center();
            input.Y = 3;
            input.Width = Dim.Fill(2);
            Add(input);
        }

        public override void OnLoaded()
        {
            base.OnLoaded();

            Button ok = new Button("Ok");
            ok.Clicked += () =>
            {

                if (string.IsNullOrWhiteSpace(input.Text.ToString()))
                {
                    MessageBox.ErrorQuery(Title, "Input cannot be empty.", "Ok");
                    return;
                }

                Input = input.Text.ToString();
                Application.RequestStop();
            };

            AddButton(ok);

            Button cancel = new Button("Cancel");
            cancel.Clicked += () =>
            {
                Input = null;
                Application.RequestStop();
            };

            AddButton(cancel);

            input.SetFocus();
        }
    }
}
