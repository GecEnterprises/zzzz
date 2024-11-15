use std::thread;
use ringbuf::{CachingCons, CachingProd, SharedRb};
use ringbuf::storage::Heap;
use ringbuf::traits::Split;
use crate::audioio::AudioFsPlayer;
use crate::outputdev::OutHandler;

mod outputdev;
mod audioio;

fn main() {
    tracing_subscriber::fmt::init();

    let (prod, cons) = SharedRb::<Heap<f32>>::new(1024 * 8 *8* 8* 8* 8 * 8).split();

    let mut player = AudioFsPlayer::load("./resources/gec.mp3", prod).unwrap();
    let mut device = OutHandler::init().unwrap();

    thread::spawn(move || {
        device.add_ringbuf(cons);
        device.open_stream().unwrap();
    });

    thread::spawn(move || {
        player.arm();
    });

    loop {
    }
}