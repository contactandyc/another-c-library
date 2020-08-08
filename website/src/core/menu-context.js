import React, { createContext, useState } from 'react';

const MenuContext = createContext(false);

function MenuProvider({ children }) {
	const [menu, setMenu] = useState(false);

	const toggleMenu = (bool) => setMenu((menu) => bool);

	return (
		<MenuContext.Provider value={{ menu, toggleMenu }}>
			{children}
		</MenuContext.Provider>
	);
}

export { MenuProvider, MenuContext };
