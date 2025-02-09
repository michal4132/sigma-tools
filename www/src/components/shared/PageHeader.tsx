import { ComponentChildren } from 'preact'

interface PageHeaderProps {
  title: string
  MenuButton: () => JSX.Element
  actions?: ComponentChildren
}

export function PageHeader({ title, MenuButton, actions }: PageHeaderProps) {
  return (
    <div className="flex items-center justify-between gap-4 mb-6">
      <div className="flex items-center gap-4">
        <MenuButton />
        <h2 className="text-2xl font-bold text-text-primary">{title}</h2>
      </div>
      {actions}
    </div>
  )
}
