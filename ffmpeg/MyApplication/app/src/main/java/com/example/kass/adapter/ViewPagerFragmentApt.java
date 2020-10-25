package com.example.kass.adapter;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentManager;
import androidx.fragment.app.FragmentStatePagerAdapter;

import com.example.kass.view.fragment.BaseFragment;

import java.util.ArrayList;
import java.util.List;

public class ViewPagerFragmentApt extends FragmentStatePagerAdapter {
    private ArrayList<BaseFragment> mFragmentList;
    private FragmentManager mFragmentManager;

    public ViewPagerFragmentApt(@NonNull FragmentManager fm, ArrayList<BaseFragment> fragments) {
        super(fm);
        mFragmentList = fragments;
    }

    public ViewPagerFragmentApt(@NonNull FragmentManager fm, int behavior) {
        super(fm, behavior);
        mFragmentManager = fm;
    }

    @NonNull
    @Override
    public Fragment getItem(int position) {
        return mFragmentList.get(position);
    }

    @Override
    public int getCount() {
        return mFragmentList.size();
    }

    public void updateList(ArrayList<BaseFragment> fragments) {
        this.mFragmentList = fragments;
        notifyDataSetChanged();
    }
}
