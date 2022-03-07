export default class Modal {
  constructor(content) {
    this.modal = document.createElement('div');
    this.modal.id = 'modal';
    this.modal.className = 'modal';

    let modalBox = document.createElement('div');
    modalBox.className = 'modal-box';

    this.modalContent = document.createElement('div');
    this.modalContent.className = 'modal-content';
    this.modalContent.appendChild(content);

    this.modalButtons = document.createElement('div');
    this.modalButtons.className = 'modal-buttons';

    modalBox.appendChild(this.modalContent);
    modalBox.appendChild(this.modalButtons);
    this.modal.appendChild(modalBox);
    document.body.appendChild(this.modal);
  }

  addButton(text, handler, opts) {
    let button = document.createElement('button');
    button.innerText = text;
    button.addEventListener('click', handler);
    if (opts.color) button.style.setProperty('--button-color', opts.color);
    if (opts.textColor)
      button.style.setProperty('--button-text-color', opts.textColor);
    this.modalButtons.appendChild(button);
  }

  remove() {
    if (this.modal == null) return;
    document.body.removeChild(this.modal);
    this.modal = null;
  }
}
