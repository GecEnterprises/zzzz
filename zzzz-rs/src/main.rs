mod gui;
mod aud;
mod shared;

use std::{
    sync::Arc,
    thread,
};

use std::sync::RwLock;
use crate::shared::SharedState;
use crossbeam_channel::{bounded};

fn main() {
    let shared_state = Arc::new(RwLock::new(SharedState {
        volume: 0.5,
        frequency: 200f32,
        is_playing: true,
    }));

    let (audio_tx, audio_rx) = bounded(32);
    let audio_state = shared_state.clone();

    // Spawn audio thread
    thread::spawn(move || {
        aud::run_audio_thread(audio_state, audio_rx).unwrap();
    });

    // GUI rendering loop (placeholder)
    gui::render_gui(shared_state.clone(), audio_tx);
}
