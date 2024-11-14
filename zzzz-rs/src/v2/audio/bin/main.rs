use std::thread;
use crate::audioio::AudioFsPlayer;
use crate::outputdev::OutHandler;

mod outputdev;
mod audioio;
mod audio;

fn main() {
    tracing_subscriber::fmt::init();

    let player = AudioFsPlayer::load("./resources/gec.mp3").unwrap();

    let device = OutHandler::init().unwrap();

    thread::spawn(move || {
        device.open_stream().unwrap();
    });

    loop {
    }
}