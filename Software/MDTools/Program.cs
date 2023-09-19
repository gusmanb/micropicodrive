using MicroDriveTools.Classes;

MicroDriveDirectory? currentDirectory = null;

Console.WriteLine("Welcome to MicroDrive Tools V1.0");

while (MainMenu()) ;

Console.WriteLine("Bye!");

bool MainMenu()
{
    Console.WriteLine();
    Console.WriteLine("Choose an option:");
    Console.WriteLine();
    Console.WriteLine("1. Create new directory");
    Console.WriteLine("2. Import cartridge");
    Console.WriteLine("3. Import zip file");
    Console.WriteLine("4. Import regular file");
    Console.WriteLine("5. View current directory");
    Console.WriteLine("6. Remove file from directory");
    Console.WriteLine("7. Export cartridge");
    Console.WriteLine("8. Exit");
    Console.WriteLine();

    Console.Write("Option: ");
    switch(Console.ReadLine())
    {
        case "1":
            CreateNewDirectory();
            return true;
        case "2":
            ImportCartridge();
            return true;
        case "3":
            ImportZIPFile();
            return true;
        case "4":
            ImportRegularFile();
            return true;
        case "5":
            ViewDirectory();
            return true;
        case "6":
            RemoveFile();
            return true;
        case "7":
            ExportCartridge();
            return true;
        case "8":
            return false;
        default:
            Console.WriteLine("Invalid option");
            return true;
    }
}
void ExportCartridge()
{
    try
    {
        if (currentDirectory == null)
        {
            Console.WriteLine("There is no directory loaded. Do you want to create a new one? (Y/N)");
            if (Console.ReadLine()?.ToUpper() != "Y")
                return;

            CreateNewDirectory();
        }

        Console.WriteLine("Enter cartridge file name:");
        string fileName = Console.ReadLine();

        Console.WriteLine("Enter medium name:");
        string mediumName = Console.ReadLine();

        MicroDriveCartridge cartridge = new MicroDriveCartridge(currentDirectory, mediumName);
        cartridge.SaveMDV(fileName);

        Console.WriteLine("Cartridge saved.");
    }
    catch(Exception ex)
    {
        Console.WriteLine($"Error exporting cartridge: {ex.Message}");
    }

}
void RemoveFile()
{
    try
    {
        if (currentDirectory == null)
        {
            Console.WriteLine("There is no directory loaded. Do you want to create a new one? (Y/N)");
            if (Console.ReadLine()?.ToUpper() != "Y")
                return;

            CreateNewDirectory();
        }

        Console.WriteLine("Enter file name:");

        string fileName = Console.ReadLine();

        if (!currentDirectory.RemoveFile(fileName))
            Console.WriteLine("File not found.");
        else
            Console.WriteLine("File removed from directory.");
    }
    catch(Exception ex)
    {
        Console.WriteLine($"Error removing file: {ex.Message}");
    }
}
void ViewDirectory()
{
    try
    {
        if (currentDirectory == null)
        {
            Console.WriteLine("There is no directory loaded. Do you want to create a new one? (Y/N)");
            if (Console.ReadLine()?.ToUpper() != "Y")
                return;

            CreateNewDirectory();
        }

        Console.WriteLine("Directory contents:");

        Console.WriteLine();

        foreach (var file in currentDirectory.Files)
        {
            Console.WriteLine($"{file.Header.FileName} ({file.Header.FileLength} bytes, {(file.Header.FileType == 1 ? "Executable" : "Regular file")})");
        }

        Console.WriteLine();

        Console.WriteLine("Listing complete.");
    }
    catch(Exception ex)
    {
        Console.WriteLine($"Error viewing directory: {ex.Message}");
    }
}
void ImportRegularFile()
{
    try
    {
        if (currentDirectory == null)
        {
            Console.WriteLine("There is no directory loaded. Do you want to create a new one? (Y/N)");
            if (Console.ReadLine()?.ToUpper() != "Y")
                return;

            CreateNewDirectory();
        }

        Console.WriteLine("Enter file name:");
        string fileName = Console.ReadLine();

        if (!File.Exists(fileName))
        {
            Console.WriteLine("File not found.");
            return;
        }

        string storeName = Path.GetFileName(fileName).Replace(".", "_");

        Console.WriteLine($"Enter file name for storage (default: {storeName}):");

        string? storeName2 = Console.ReadLine();

        if (!string.IsNullOrEmpty(storeName2))
            storeName = storeName2;

        byte[] data = File.ReadAllBytes(fileName);
        Console.WriteLine("Is this file executable? (Y/N)");

        if (Console.ReadLine()?.ToUpper() == "Y")
        {
            Console.WriteLine("Enter data space:");
            uint dataSpace = uint.Parse(Console.ReadLine() ?? "0");

            currentDirectory.AddFile(new MicroDriveFile(storeName, data, true, dataSpace));
        }
        else
            currentDirectory.AddFile(new MicroDriveFile(storeName, data));

        Console.WriteLine("File added to directory.");
    }
    catch(Exception ex)
    {
        Console.WriteLine($"Error importing file: {ex.Message}");
    }
}
void ImportZIPFile()
{
    try
    {
        Console.WriteLine("Enter zip file name:");
        string fileName = Console.ReadLine();

        if (!File.Exists(fileName))
        {
            Console.WriteLine("File not found.");
            return;
        }

        MicroDriveDirectory directory = ZIPImporter.ImportZIPFile(fileName);
        if (currentDirectory == null)
        {
            currentDirectory = directory;
            Console.WriteLine("Directory loaded from zip file.");
        }
        else
        {
            foreach (var file in directory.Files)
                currentDirectory.AddFile(file);

            Console.WriteLine("Zip file merged with directory.");
        }
    }
    catch(Exception ex)
    {
        Console.WriteLine($"Error importing zip file: {ex.Message}");
    }
}
void ImportCartridge()
{
    try
    {
        Console.WriteLine("Enter cartridge file name:");
        string fileName = Console.ReadLine();

        if (!File.Exists(fileName))
        {
            Console.WriteLine("File not found.");
            return;
        }

        MicroDriveCartridge cartridge = MicroDriveCartridge.LoadMDV(fileName);
        if (currentDirectory == null)
        {
            currentDirectory = cartridge.Directory;
            Console.WriteLine("Directory imported from cartridge.");
        }
        else
        {
            foreach (var file in cartridge.Directory.Files)
                currentDirectory.AddFile(file);

            Console.WriteLine("Cartridge directory merged with current directory.");
        }
    }
    catch(Exception ex)
    {
        Console.WriteLine($"Error importing cartridge: {ex.Message}");
    }
}
void CreateNewDirectory()
{
    try
    {
        if (currentDirectory != null)
        {
            Console.WriteLine("There is already a directory loaded. Do you want to discard it? (Y/N)");
            if (Console.ReadLine()?.ToUpper() != "Y")
                return;
        }

        currentDirectory = new MicroDriveDirectory();

        Console.WriteLine("Directory created.");
    }
    catch(Exception ex)
    {
        Console.WriteLine($"Error creating directory: {ex.Message}");
    }
}