import { motion } from 'framer-motion'
import MetricBar from './shared/MetricBar'
import BoxWrapper from './shared/BoxWrapper'

// Tutte le misure qui sotto arrivano dall'analyzer del plugin. Prima di
// agosto 2026 alcune erano ricavate per stima dai valori vicini — RMS come
// "LUFS + 2.1", true peak come "peak + 0.3", una gamma dinamica dedotta dai
// meter — e mostravano numeri plausibili ma falsi.
export default function LoudnessBox({ lufs, lufsShort = -60, peak, rms = -60, onDawCmd, ...rest }) {
  const hasSignal = peak > -60 || rms > -60

  // Crest factor = picco meno RMS. Con LUFS al posto dell'RMS il numero
  // esce diverso: il LUFS è pesato in frequenza, l'RMS no.
  const crest = hasSignal ? peak - rms : 0
  // Quanto manca prima di toccare lo zero.
  const headroom = hasSignal ? -peak : 0

  const fmt = (v, unit) => (hasSignal && v > -60 ? `${v.toFixed(1)} ${unit}` : '-∞')

  return (
    <BoxWrapper label="Loudness Analysis" color="#00E5FF" icon="equalizer" {...rest}>
      <div className="grid grid-cols-2 gap-1.5">
        <MetricBar label="LUFS Integrated" val={fmt(lufs, 'LUFS')}
          color="#00E5FF" pct={Math.max(0, Math.min(100, (lufs + 60) / 60 * 100))} />
        <MetricBar label="LUFS Short-term" val={fmt(lufsShort, 'LUFS')}
          color="#00E5FF" pct={Math.max(0, Math.min(100, (lufsShort + 60) / 60 * 100))} />
        <MetricBar label="True Peak" val={fmt(peak, 'dBTP')}
          color={peak > -1 ? '#DC143C' : '#FF6B35'}
          pct={Math.max(0, Math.min(100, (peak + 60) / 60 * 100))} />
        <MetricBar label="RMS Level" val={fmt(rms, 'dB')}
          color="#FFB000" pct={Math.max(0, Math.min(100, (rms + 60) / 60 * 100))} />
        <MetricBar label="Crest Factor" val={hasSignal ? `${crest.toFixed(1)} dB` : '—'}
          color="#DC143C" pct={Math.min(100, Math.max(0, crest * 5))} />
        <MetricBar label="Headroom" val={hasSignal ? `${headroom.toFixed(1)} dB` : '—'}
          color={headroom < 1 ? '#DC143C' : '#00FFaa'}
          pct={Math.min(100, Math.max(0, headroom * 5))} />
        <div className="col-span-2 flex gap-1 pt-1">
          <motion.button whileTap={{ scale: 0.95 }}
            className="flex-1 py-1 text-[9px] font-bold uppercase tracking-wider border text-[#888] border-[#333] hover:border-[#00E5FF]"
            onClick={() => onDawCmd('targetLoudness', { target: -14 })}>
            TARGET -14 LUFS
          </motion.button>
          <motion.button whileTap={{ scale: 0.95 }}
            className="flex-1 py-1 text-[9px] font-bold uppercase tracking-wider border text-[#888] border-[#333] hover:border-[#00E5FF]"
            onClick={() => onDawCmd('targetLoudness', { target: -16 })}>
            TARGET -16 LUFS
          </motion.button>
          <motion.button whileTap={{ scale: 0.95 }}
            className="flex-1 py-1 text-[9px] font-bold uppercase tracking-wider border text-[#888] border-[#333] hover:border-[#FF6B35]"
            onClick={() => onDawCmd('limiter')}>
            LIMIT
          </motion.button>
        </div>
      </div>
    </BoxWrapper>
  )
}
