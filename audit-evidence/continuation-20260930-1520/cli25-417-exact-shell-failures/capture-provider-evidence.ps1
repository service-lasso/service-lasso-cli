$ErrorActionPreference = 'Stop'

$repo = 'service-lasso/service-lasso-cli'
$run = '36780864895'
$jobs = @('110110486450', '110110486414')

gh api "repos/$repo/actions/runs/$run" | Set-Content -NoNewline -Encoding utf8 provider-run-36780864895.json
foreach ($job in $jobs) {
  gh api "repos/$repo/actions/jobs/$job" | Set-Content -NoNewline -Encoding utf8 "provider-job-$job.json"
  gh run view $run --repo $repo --job $job --log | Set-Content -NoNewline -Encoding utf8 "raw-job-$job.log"
}

Get-FileHash provider-run-36780864895.json, provider-job-*.json, raw-job-*.log -Algorithm SHA256 |
  Sort-Object Path |
  ForEach-Object { "$($_.Hash.ToLowerInvariant())  $([IO.Path]::GetFileName($_.Path))" } |
  Set-Content -Encoding ascii SHA256SUMS
