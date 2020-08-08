// Import styles
import './src/theme.css';
import './src/app.css';
import './src/components/components.css';
import './src/templates/templates.css';

// Import React context for menu
import React from 'react';
import { MenuProvider } from './src/core/menu-context';

export const wrapRootElement = ({ element }) => (
	<MenuProvider>{element}</MenuProvider>
);
