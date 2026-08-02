import MetricBar from './shared/MetricBar'
import BoxWrapper from './shared/BoxWrapper'

// Quota di energia sotto i 200 Hz, letta dallo spettro reale. I bin
// trasmessi vengono da una FFT a 2048 punti, quindi ognuno copre
// sampleRate/2048 Hz — a 48 kHz sono circa 23 Hz per bin.
function lowEndShare(spectrum, sampleRate = 48000) {
  if (!spectrum || spectrum.length === 0) return null
  const binHz = sampleRate / 2048
  const cutoffBin = Math.max(1, Math.round(200 / binHz))
  let low = 0, total = 0
  for (let i = 0; i < spectrum.length; i++) {
    const e = spectrum[i] * spectrum[i]
    total += e
    if (i < cutoffBin) low += e
  }
  if (total < 1e-9) return null
  return Math.round((low / total) * 100)
}

export default function MetricsBox({ meterL, meterR, lufs, peak, rms = -60, spectrum = [], sampleRate, ...rest }) {
  const lPct = Math.round(meterL * 100)
  const rPct = Math.round(meterR * 100)

  const hasSignal = peak > -60 || rms > -60
  // Crest factor vero: picco meno RMS.
  const crest = hasSignal ? peak - rms : 0
  const lowEnd = lowEndShare(spectrum, sampleRate)

  return (
    <BoxWrapper label="Mix Analysis" color="#FFB000" icon="analytics" {...rest}>
      <div className="grid grid-cols-2 gap-1.5">
        <MetricBar label="L/R Balance"
          val={lPct > rPct ? `${lPct - rPct}% L` : rPct > lPct ? `${rPct - lPct}% R` : 'CENTER'}
          color="#DC143C" pct={Math.abs(lPct - rPct)} />
        <MetricBar label="RMS" val={hasSignal && rms > -60 ? `${rms.toFixed(1)} dB` : '—'}
          color="#FFB000" pct={Math.max(0, Math.min(100, (rms + 60) / 60 * 100))} />
        <MetricBar label="LUFS" val={lufs > -60 ? `${lufs.toFixed(1)}` : '—'}
          color="#00E5FF" pct={Math.max(0, Math.min(100, (lufs + 60) / 60 * 100))} />
        <MetricBar label="True Peak" val={peak > -60 ? `${peak.toFixed(1)} dB` : '—'}
          color={peak > -1 ? '#DC143C' : '#FF6B35'}
          pct={Math.max(0, Math.min(100, (peak + 60) / 60 * 100))} />
        <MetricBar label="Crest Factor" val={hasSignal ? `${crest.toFixed(1)} dB` : '—'}
          color="#00FFaa" pct={Math.min(100, Math.max(0, crest * 5))} />
        <MetricBar label="Low End" val={lowEnd !== null ? `${lowEnd}%` : '—'}
          color="#DC143C" pct={lowEnd ?? 0} />
      </div>
    </BoxWrapper>
  )
}
