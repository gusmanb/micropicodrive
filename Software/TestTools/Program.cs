using MicroDriveTools.Classes;
using MicroDriveTools.Structs;

//var dir1 = ZIPImporter.ImportZIPFile("mmetropolism.zip");

//var md = new MicroDriveCartridge(dir1, "mmetropolism");

//var dir = new MicroDriveDirectory();

//byte[] b = File.ReadAllBytes("hola");

//dir.AddFile(new MicroDriveFile("un_archivo", b));

//foreach(var file in dir1.Files)
//{
//    dir.AddFile(new MicroDriveFile(file.Header.FileName, file.Data));
//}

//var md = new MicroDriveCartridge(dir, "cosmic");

//md.SaveMDV("mmetropolism.mdv");

/*
var cart1 = MicroDriveCartridge.LoadMDV("pepe.mdv");

var s0md = md.Sectors.Where(s => s.Header.SectorNumber == 0).FirstOrDefault();
var s0ct = cart1.Sectors.Where(s => s.Header.SectorNumber == 0).FirstOrDefault();

unsafe 
{
    byte* data1 = (byte*)&s0md;
    byte* data2 = (byte*)&s0ct;

    for(int buc = 0; buc < sizeof(MicroDriveSector); buc++)
    {
        if (data1[buc] != data2[buc])
            Console.WriteLine($"Byte {buc} differs: {data1[buc]} - {data2[buc]}");
    }

    Console.WriteLine("Random in sectors");

    foreach (var sector in md.Sectors)
    {
        byte* data3 = (byte*)&sector;
        Console.WriteLine($"{data3[12]}{data3[13]}");
    }
}



foreach (var sector in cart1.Sectors)
{
    var sect2 = md.Sectors.Cast<MicroDriveSector?>().Where(s => s.Value.Header.SectorNumber == sector.Header.SectorNumber).FirstOrDefault();

    if(sect2 == null)
    {
        Console.WriteLine($"Sector {sector.Header.MediumName}-{sector.Header.SectorNumber} not found in cart2");
        continue;
    }

    if (!sect2.Value.Header.Equals(sector.Header))
    {
        Console.WriteLine($"Sector {sector.Header.MediumName}-{sector.Header.SectorNumber} differs in header");
        Console.WriteLine($"{sector.Header.HeaderFlag}-{sector.Header.SectorNumber}-{sector.Header.MediumName}/{sect2.Value.Header.HeaderFlag}-{sect2.Value.Header.SectorNumber}-{sect2.Value.Header.MediumName}");
    }
    if (!sect2.Value.Record.Equals(sector.Record))
    {
        Console.WriteLine($"Sector {sector.Header.MediumName}-{sector.Header.SectorNumber} differs in record");
    }
}
/*
*/

//var cart = MicroDriveCartridge.LoadMDV("empty.mdv");
//var cart = MicroDriveCartridge.LoadMDV("fmt.mdv");
var cart = MicroDriveCartridge.LoadMDV("d:\\t_fix.mdv");


Console.WriteLine($"Cartridge name: {cart.MediumName}");

Console.WriteLine($"Free sectors: {cart.Map.FreeSectors.Length}");
Console.WriteLine($"Damaged sectors: {cart.Map.DamagedSectors.Length}");

foreach(var sector in cart.Map.DamagedSectors)
    Console.WriteLine($"  - Sector {sector.SectorNumber}, file {sector.FileNumber}, block {sector.FileBlock}");

Console.WriteLine($"Last allocated sector: {cart.Map.LastAllocatedSector}");

Console.WriteLine($"--DIRECTORY MAP--");

foreach (var file in cart.Directory.Files)
{
    Console.WriteLine($"File: {file.Header.FileName}, Size: {file.Header.FileLength}, Type: {file.Header.FileType}, DataSpace: {file.Header.DataSpace}, ExtraInfo: {file.Header.ExtraInfo}, UpdateDate: {file.Header.UpdateDate}, ReferenceDate: {file.Header.ReferenceDate}, BackupDate: {file.Header.BackupDate}");
    Console.WriteLine("File map:");

    var map = cart.Map.GetFileMap(file.FileNumber);

    foreach (var entry in map)
        Console.WriteLine($"    - Sector {entry.SectorNumber}, Block {entry.FileBlock}");

}

Console.WriteLine($"--RAW SECTORS--");

var mapEntries = cart.Map.ToArray();

foreach (var sector in cart.Sectors)
{
    Console.WriteLine($"Sector { sector.Header.MediumName + "-" + sector.Header.SectorNumber}, File {sector.Record.FileNumber + "-" + sector.Record.FileBlock}, Flag { sector.Header.HeaderFlag}");

    var entry = mapEntries.Cast<MicroDriveSectorMapEntry?>().Where(e => e.Value.SectorNumber == sector.Header.SectorNumber).FirstOrDefault();

    Console.WriteLine($"Sector map: { (entry == null ? "NULL" : $"{entry.Value.FileNumber}-{entry.Value.FileBlock}") }");

    if(entry?.FileNumber != sector.Record.FileNumber || entry?.FileBlock != sector.Record.FileBlock)
    {
        Console.WriteLine($"Sector {sector.Header.MediumName}-{sector.Header.SectorNumber} has a different file number/block in the map: {entry?.FileNumber}:{entry?.FileBlock} vs {sector.Record.FileNumber}:{sector.Record.FileBlock}");
    }
}   