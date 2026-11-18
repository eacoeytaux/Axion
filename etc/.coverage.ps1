# https://stackoverflow.com/a/70321555

function Write-Newline { Write-Host "" }

Write-Newline
Write-Host "checking install..."
Write-Newline

# This only needs to be installed once (globally), if installed it fails silently 
dotnet tool install -g dotnet-reportgenerator-globaltool

Write-Newline
Write-Host "starting coverage..."
Write-Newline

$dir = (get-item $PSScriptRoot ).parent.FullName
$dirTest = $dir + "\cov"

Set-Location $dir

# Run the Coverlet.Collector
$output = [string] (& dotnet test Axion.sln --collectXPlat Code Coverage 2&1)
Write-Host Last Exit Code $lastexitcode
Write-Host $output

# Delete previous test run reports - note if you're getting wrong results do a Solution Clean and Rebuild to remove stale DLLs in the bin folder
Remove-Item -Recurse -Force $dir/coveragereport/

# To keep a history of the Code Coverage we need to use the argument -historydirSOME_DIRECTORY 
if (!(Test-Path -path $dir/CoverageHistory)) { New-Item -ItemType directory -Path $dir/CoverageHistory }

# Generate the Code Coverage HTML Report
reportgenerator -reports$dircoverage.cobertura.xml -targetdir$dir/coveragereport -reporttypesHtml -historydir$dir/CoverageHistory 

# Open the Code Coverage HTML Report (if running on a WorkStation)
$osInfo = Get-CimInstance -ClassName Win32_OperatingSystem
if ($osInfo.ProductType -eq 1) { (& $dir/coveragereportindex.html) }

Write-Newline
Write-Host "finished!"
Write-Host 'press any key to exit...';
$null = $Host.UI.RawUI.ReadKey('NoEcho,IncludeKeyDown');
