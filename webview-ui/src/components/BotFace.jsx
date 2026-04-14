import React from 'react';
import { motion } from 'framer-motion';

export const BotStates = {
  IDLE: 'idle',
  LISTENING: 'listening',
  THINKING: 'thinking',
  TYPING: 'typing',
  ERROR: 'error',
  SUCCESS: 'success',
  LOADING: 'loading',
  ADVISORY: 'advisory',
  SAD: 'sad'
};

export function BotFace({ state = 'idle', className = 'w-16 h-16' }) {
  const colors = {
    idle: '#00E5FF',
    listening: '#FFB000',
    thinking: '#FFB000',
    typing: '#00E5FF',
    error: '#DC143C',
    success: '#00FFaa',
    loading: '#00E5FF',
    advisory: '#FFB000',
    sad: '#4169E1'
  };

  const color = colors[state] || colors.idle;

  const paths = {
    idle: {
      l: 'M 28 45 Q 35 35 42 45',
      r: 'M 58 45 Q 65 35 72 45',
      m: 'M 40 65 Q 50 72 60 65',
      y: 0
    },
    sad: {
      l: 'M 25 40 Q 35 48 45 48',
      r: 'M 55 48 Q 65 48 75 40',
      m: 'M 40 70 Q 50 62 60 70',
      y: 8
    },
    thinking: {
      l: 'M 32 40 Q 37 35 42 40',
      r: 'M 62 40 Q 67 35 72 40',
      m: 'M 46 65 Q 50 65 54 65',
      y: -5
    },
    typing: {
      l: 'M 28 42 Q 35 32 42 42',
      r: 'M 58 42 Q 65 32 72 42',
      m: 'M 38 62 Q 50 78 62 62',
      y: -2
    },
    listening: {
      l: 'M 26 42 Q 35 30 44 42',
      r: 'M 56 42 Q 65 30 74 42',
      m: 'M 40 65 Q 50 65 60 65',
      y: 0
    },
    error: {
      l: 'M 28 40 Q 35 50 42 40',
      r: 'M 58 40 Q 65 50 72 40',
      m: 'M 38 68 Q 50 58 62 68',
      y: 2
    },
    success: {
      l: 'M 25 45 Q 35 25 45 45',
      r: 'M 55 45 Q 65 25 75 45',
      m: 'M 35 60 Q 50 85 65 60',
      y: -6
    },
    loading: {
      l: 'M 30 45 Q 35 45 40 45',
      r: 'M 60 45 Q 65 45 70 45',
      m: 'M 45 65 Q 50 65 55 65',
      y: 0
    },
    advisory: {
      l: 'M 28 42 Q 35 38 42 42',
      r: 'M 58 42 Q 65 38 72 42',
      m: 'M 42 65 Q 50 65 58 65',
      y: 0
    }
  };

  const current = paths[state] || paths.idle;

  const mouthAnimate = state === 'typing'
    ? [current.m, 'M 42 64 Q 50 68 58 64', current.m]
    : state === 'listening'
    ? [current.m, 'M 35 65 Q 50 65 65 65', current.m]
    : state === 'loading'
    ? [current.m, 'M 40 65 Q 50 65 60 65', current.m]
    : current.m;

  const mouthTransition = (state === 'typing' || state === 'listening' || state === 'loading')
    ? { repeat: Infinity, duration: 0.6, ease: 'easeInOut' }
    : { duration: 0.6, type: 'spring', bounce: 0.5 };

  const eyeAnimateL = state === 'idle' ? [current.l, current.l, 'M 28 45 Q 35 45 42 45', current.l] : current.l;
  const eyeAnimateR = state === 'idle' ? [current.r, current.r, 'M 58 45 Q 65 45 72 45', current.r] : current.r;
  const eyeTransition = state === 'idle'
    ? { repeat: Infinity, duration: 5, times: [0, 0.95, 0.97, 1] }
    : { duration: 0.6, type: 'spring', bounce: 0.5 };

  return (
    <motion.div
      className={`relative flex items-center justify-center ${className}`}
      animate={{ y: current.y }}
      transition={{ type: 'spring', stiffness: 80, damping: 12 }}
    >
      <motion.svg
        viewBox="0 0 100 100"
        className="w-full h-full drop-shadow-[0_0_12px_currentColor]"
        style={{ color }}
        animate={{ y: [0, -3, 0] }}
        transition={{ repeat: Infinity, duration: 4, ease: 'easeInOut' }}
      >
        <motion.g stroke="currentColor" strokeWidth="4.5" strokeLinecap="round" fill="none">
          <motion.path
            d={current.l}
            animate={{ d: eyeAnimateL }}
            transition={eyeTransition}
          />
          <motion.path
            d={current.r}
            animate={{ d: eyeAnimateR }}
            transition={eyeTransition}
          />
          <motion.path
            d={current.m}
            animate={{ d: mouthAnimate }}
            transition={mouthTransition}
          />
        </motion.g>
      </motion.svg>
    </motion.div>
  );
}