import { useState } from 'react'
import { motion } from 'framer-motion'
import MetricBar from './shared/MetricBar'
import BoxWrapper from './shared/BoxWrapper'

// Mid e side ricavati dai campioni veri del vectorscope, non stimati dai
// livelli dei meter. Prima la correlazione qui era "0.62 + (L+R) * 0.15":
// un numero sempre positivo per costruzione, che non poteva mai segnalare
// un problema di fase — cioè esattamente ciò per cui la si guarda.
function stereoEnergy(points) {
  if (!points || points.length < 2) return null
  let sumSide = 0, sumMid = 0, n = 0
  for (let i = 0; i + 1 < points.length; i += 2) {
    sumSide += points[i] * points[i]
    sumMid  += points[i + 1] * points[i + 1]
    n++
  }
  if (n === 0) return null
  const side = Math.sqrt(sumSide / n)
  const mid  = Math.sqrt(sumMid / n)
  if (side + mid < 1e-6) return null
  return { side, mid }
}

export default function StereoscopeBox({ meterL, meterR, correlation = 0, scopePoints = [], onDawCmd, ...rest }) {
  const [midSide, setMidSide] = useState(false)
  const lPct = Math.round(meterL * 100)
  const rPct = Math.round(meterR * 100)

  const energy = stereoEnergy(scopePoints)
  // Quota di energia che sta sul lato: 0% mono puro, 100% tutto fuori fase.
  const sidePct = energy ? Math.round((energy.side / (energy.side + energy.mid)) * 100) : 0
  // Larghezza come rapporto side/mid, limitata a 200% per non uscire dalla barra.
  const widthPct = energy && energy.mid > 1e-6
    ? Math.min(200, Math.round((energy.side / energy.mid) * 100))
    : (energy ? 200 : 0)

  const corrColor = correlation < 0 ? '#DC143C' : correlation < 0.5 ? '#FFB000' : '#00E5FF'

  return (
    <BoxWrapper label="Stereo Field" color="#DC143C" icon="swap_horiz" {...rest}>
      <div className="grid grid-cols-2 gap-1.5">
        <MetricBar label="L/R Balance" val={lPct > rPct ? `${lPct - rPct}% L` : rPct > lPct ? `${rPct - lPct}% R` : 'CENTER'}
          color="#DC143C" pct={Math.abs(lPct - rPct)} />
        <MetricBar label="Side Content" val={energy ? `${sidePct}%` : '—'}
          color="#FFB000" pct={sidePct} />
        <MetricBar label="Correlation" val={energy ? correlation.toFixed(2) : '—'}
          color={corrColor} pct={Math.max(0, Math.min(100, (correlation + 1) * 50))} />
        <MetricBar label="Width" val={energy ? `${widthPct}%` : '—'}
          color="#00FFaa" pct={Math.min(100, widthPct / 2)} />
        <div className="col-span-2 flex gap-1 pt-1">
          <motion.button whileTap={{ scale: 0.95 }}
            className={`flex-1 py-1 text-[9px] font-bold uppercase tracking-wider border ${midSide ? 'bg-[#DC143C] text-black border-[#DC143C]' : 'text-[#888] border-[#333] hover:border-[#DC143C]'}`}
            onClick={() => { setMidSide(!midSide); onDawCmd('midSide', { enabled: !midSide }) }}>
            MID/SIDE
          </motion.button>
          <motion.button whileTap={{ scale: 0.95 }}
            className="flex-1 py-1 text-[9px] font-bold uppercase tracking-wider border text-[#888] border-[#333] hover:border-[#00FFaa]"
            onClick={() => onDawCmd('narrow')}>
            NARROW
          </motion.button>
          <motion.button whileTap={{ scale: 0.95 }}
            className="flex-1 py-1 text-[9px] font-bold uppercase tracking-wider border text-[#888] border-[#333] hover:border-[#FFB000]"
            onClick={() => onDawCmd('widen')}>
            WIDEN
          </motion.button>
        </div>
      </div>
    </BoxWrapper>
  )
}
