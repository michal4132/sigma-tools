import { ComponentChildren } from 'preact'

interface CardProps {
  title: string
  children: ComponentChildren
  className?: string
}

export function Card({ title, children, className = '' }: CardProps) {
  return (
    <div className={`bg-bg-secondary p-6 rounded-lg border border-bg-accent ${className}`}>
      <h3 className="text-lg font-semibold text-text-primary">{title}</h3>
      <div className="mt-4">
        {children}
      </div>
    </div>
  )
}
