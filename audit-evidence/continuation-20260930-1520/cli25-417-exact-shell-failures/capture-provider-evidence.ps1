$ErrorActionPreference = 'Stop'

$repo = 'service-lasso/service-lasso-cli'
$run = '36780864895'
$jobs = @('110110486450', '110110486414')
$utf8 = [Text.UTF8Encoding]::new($false)

[IO.File]::WriteAllText((Join-Path $PWD 'provider-run-36780864895.json'), (gh api "repos/$repo/actions/runs/$run"), $utf8)
foreach ($job in $jobs) {
  [IO.File]::WriteAllText((Join-Path $PWD "provider-job-$job.json"), (gh api "repos/$repo/actions/jobs/$job"), $utf8)
  [IO.File]::WriteAllText((Join-Path $PWD "raw-job-$job.log"), ((gh run view $run --repo $repo --job $job --log) -join "`n") + "`n", $utf8)
}

 $sums = Get-FileHash provider-run-36780864895.json, provider-job-*.json, raw-job-*.log -Algorithm SHA256 |
  Sort-Object Path |
  ForEach-Object { "$($_.Hash.ToLowerInvariant())  $([IO.Path]::GetFileName($_.Path))" }
[IO.File]::WriteAllText((Join-Path $PWD 'SHA256SUMS'), ($sums -join "`n") + "`n", $utf8)
