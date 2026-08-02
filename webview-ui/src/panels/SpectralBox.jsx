import { motion } from 'framer-motion'
import BoxWrapper from './shared/BoxWrapper'

export default function SpectralBox({ spectrum, onDawCmd, ...rest }) {
  // Senza segnale si mostrano barre piatte. Prima al loro posto c'era uno
  // spettro finto fatto con Math.random(), che si muoveva anche a plugin
  // scollegato e faceva sembrare che stesse analizzando qualcosa.
  const hasSignal = spectrum.length > 0
  const bins = hasSignal ? spectrum.slice(0, 128) : Array(128).fill(0)

  return (
    <BoxWrapper label="Spectral Analyzer" color="#00E5FF" icon="finance" {...rest}>
      <div className="space-y-1.5">
        <div className="h-16 flex items-end gap-[1px] relative">
          {bins.map((mag, i) => (
            <motion.div key={i} className="flex-1 rounded-t-sm"
              style={{
                backgroundColor: hasSignal
                  ? `hsl(${200 + (1 - mag) * 120}, 85%, ${20 + mag * 45}%)`
                  : '#1a1a1a',
                opacity: 0.85,
              }}
              animate={{ height: `${Math.max(3, mag * 100)}%` }}
              transition={{ duration: 0.06 }}
            />
          ))}
          {!hasSignal && (
            <span className="absolute inset-0 flex items-center justify-center text-[8px] font-mono text-[#555]">
              nessun segnale
            </span>
          )}
        </div>
        <div className="flex justify-between text-[8px] font-mono text-[#555]">
          <span>20Hz</span><span>100Hz</span><span>500Hz</span><span>2kHz</span><span>10kHz</span><span>20kHz</span>
        </div>
        <div className="flex gap-1 pt-1">
          <motion.button whileTap={{ scale: 0.95 }}
            className="flex-1 py-1 text-[9px] font-bold uppercase tracking-wider border text-[#888] border-[#333] hover:border-[#00E5FF]"
            onClick={() => onDawCmd('eqAnalyze')}>ANALYZE</motion.button>
          <motion.button whileTap={{ scale: 0.95 }}
            className="flex-1 py-1 text-[9px] font-bold uppercase tracking-wider border text-[#888] border-[#333] hover:border-[#FFB000]"
            onClick={() => onDawCmd('eqMatch', { target: 'reference' })}>MATCH EQ</motion.button>
          <motion.button whileTap={{ scale: 0.95 }}
            className="flex-1 py-1 text-[9px] font-bold uppercase tracking-wider border text-[#888] border-[#333] hover:border-[#DC143C]"
            onClick={() => onDawCmd('spectralAnalyze')}>FFT</motion.button>
        </div>
      </div>
    </BoxWrapper>
  )
}
